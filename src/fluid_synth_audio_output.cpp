#include "fluid_synth_audio_output.hpp"

#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <QMediaDevices>
#include <QThread>
#include <QDebug>
#include <algorithm>
#include <chrono>
#include <vector>

namespace {
constexpr int kAudioSampleRate = 44100;
constexpr int kAudioChannelCount = 2;
constexpr qsizetype kRequestedBufferFrames = 2048;
constexpr qsizetype kTargetBufferedFrames = 1024;
constexpr qsizetype kRenderChunkFrames = 512;
constexpr auto kWaitInterval = std::chrono::milliseconds(1);

QString describeAudioFormat(const QAudioFormat& format)
{
    return QStringLiteral("%1 Hz, %2 channels, sample format %3")
        .arg(format.sampleRate())
        .arg(format.channelCount())
        .arg(static_cast<int>(format.sampleFormat()));
}
} // namespace

FluidSynthAudioOutput::FluidSynthAudioOutput()
    : m_thread([this]() { run(); })
{
    std::unique_lock lock(m_mutex);
    m_condition.wait(lock, [this]() { return m_ready; });
}

FluidSynthAudioOutput::~FluidSynthAudioOutput()
{
    {
        std::lock_guard lock(m_mutex);
        m_running = false;
    }
    m_condition.notify_one();

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

fluid_synth_t* FluidSynthAudioOutput::synth() const
{
    return m_synth;
}

void FluidSynthAudioOutput::setGain(qreal gain) const
{
    call([gain](fluid_synth_t* synth) { fluid_synth_set_gain(synth, gain); });
}

bool FluidSynthAudioOutput::loadSoundFont(const QString& path) const
{
    return call([&path](fluid_synth_t* synth) {
        if (fluid_synth_sfload(synth, path.toStdString().c_str(), 1) == FLUID_FAILED) {
            return false;
        }

        fluid_synth_program_reset(synth);
        return true;
    });
}

void FluidSynthAudioOutput::noteOn(int chan, int midiNote, int velocity) const
{
    call([=](fluid_synth_t* synth) { fluid_synth_noteon(synth, chan, midiNote, velocity); });
}

void FluidSynthAudioOutput::noteOff(int chan, int midiNote) const
{
    call([=](fluid_synth_t* synth) { fluid_synth_noteoff(synth, chan, midiNote); });
}

void FluidSynthAudioOutput::cc(int chan, int control, int value) const
{
    call([=](fluid_synth_t* synth) { fluid_synth_cc(synth, chan, control, value); });
}

void FluidSynthAudioOutput::programChange(int chan, int program) const
{
    call([=](fluid_synth_t* synth) { fluid_synth_program_change(synth, chan, program); });
}

void FluidSynthAudioOutput::pitchBend(int chan, int value) const
{
    call([=](fluid_synth_t* synth) { fluid_synth_pitch_bend(synth, chan, value); });
}

void FluidSynthAudioOutput::allNotesOff() const
{
    call([](fluid_synth_t* synth) {
        for (int channel = 0; channel < 16; ++channel) {
            fluid_synth_all_notes_off(synth, channel);
        }
    });
}

void FluidSynthAudioOutput::run()
{
    QAudioFormat format;
    format.setSampleRate(kAudioSampleRate);
    format.setChannelCount(kAudioChannelCount);
    format.setSampleFormat(QAudioFormat::Int16);

    const QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (!device.isFormatSupported(format)) {
        const QAudioFormat preferredFormat = device.preferredFormat();
        qFatal("Audio output does not support the required FluidSynth PCM format. Required: %s. Preferred: %s.",
               qPrintable(describeAudioFormat(format)),
               qPrintable(describeAudioFormat(preferredFormat)));
    }

    auto* settings = new_fluid_settings();
    fluid_settings_setnum(settings, "synth.sample-rate", format.sampleRate());

    auto* synth = new_fluid_synth(settings);
    QAudioSink audioSink(device, format);
#if QT_VERSION >= QT_VERSION_CHECK(6, 10, 0)
    audioSink.setBufferFrameCount(kRequestedBufferFrames);
#else
    audioSink.setBufferSize(kRequestedBufferFrames * format.bytesPerFrame());
#endif

    QIODevice* outputDevice = audioSink.start();
    if (outputDevice == nullptr) {
        qFatal("Failed to start QAudioSink output device.");
    }

    {
        std::lock_guard lock(m_mutex);
        m_synth = synth;
        m_ready = true;
    }
    m_condition.notify_all();

    qDebug() << "FluidSynth audio thread started with format" << describeAudioFormat(format)
             << "requested buffer frames" << kRequestedBufferFrames
             << "actual sink buffer size" << audioSink.bufferSize() << "bytes.";

    std::vector<short> pcmBuffer;
    pcmBuffer.resize(kRenderChunkFrames * format.channelCount());

    while (true) {
        std::deque<SynthCommand> commands;
        {
            std::unique_lock lock(m_mutex);
            if (!m_running) {
                break;
            }
            commands.swap(m_pendingCommands);
        }

        for (auto& command : commands) {
            command(synth);
        }

        const qsizetype bytesPerFrame = format.bytesPerFrame();
        const qsizetype targetBufferedBytes = kTargetBufferedFrames * bytesPerFrame;
        qsizetype bufferedBytes = audioSink.bufferSize() - audioSink.bytesFree();

        while (bufferedBytes < targetBufferedBytes) {
            const qsizetype framesToRender = std::min(
                kRenderChunkFrames,
                std::max<qsizetype>(1, (targetBufferedBytes - bufferedBytes + bytesPerFrame - 1) / bytesPerFrame));

            pcmBuffer.resize(framesToRender * format.channelCount());
            fluid_synth_write_s16(
                synth,
                framesToRender,
                pcmBuffer.data(), 0, format.channelCount(),
                pcmBuffer.data(), 1, format.channelCount());

            const qsizetype bytesToWrite = framesToRender * bytesPerFrame;
            qsizetype totalWritten = 0;
            const char* data = reinterpret_cast<const char*>(pcmBuffer.data());

            while (totalWritten < bytesToWrite) {
                const qint64 written = outputDevice->write(data + totalWritten, bytesToWrite - totalWritten);
                if (written <= 0) {
                    break;
                }
                totalWritten += written;
            }

            if (totalWritten <= 0) {
                break;
            }

            bufferedBytes += totalWritten;
        }

        std::unique_lock lock(m_mutex);
        if (!m_running) {
            break;
        }
        m_condition.wait_for(lock, kWaitInterval, [this]() {
            return !m_running || !m_pendingCommands.empty();
        });
    }

    audioSink.stop();

    {
        std::lock_guard lock(m_mutex);
        m_synth = nullptr;
    }

    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}
