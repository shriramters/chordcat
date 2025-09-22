#pragma once

#include <QObject>
#include <QIODevice>
#include <QAudioFormat>
#include <QDebug>
#include <fluidsynth.h>

class MidiSynthIODevice : public QIODevice
{
    Q_OBJECT
public:
    explicit MidiSynthIODevice(QObject* parent = nullptr)
        : QIODevice(parent)
    {
        m_settings = new_fluid_settings();
        m_synth = new_fluid_synth(m_settings);
    }

    ~MidiSynthIODevice() override
    {
        if (m_synth) {
            delete_fluid_synth(m_synth);
        }
        if (m_settings) {
            delete_fluid_settings(m_settings);
        }
    }

    fluid_synth_t* synth() const { return m_synth; }

    bool isSequential() const override
    {
        return true;
    }

    // Called by QAudioSink to read more PCM audio
    qint64 readData(char* data, qint64 maxlen) override
    {
        // Each frame (stereo 16-bit) is 4 bytes: 2 channels * 2 bytes each = 4
        const qint64 framesAvailable = maxlen / 4; // for stereo, 16-bit

        fluid_synth_write_s16(
            m_synth,
            framesAvailable,
            (void*)data, 0, 2,   // left chan offset 0, stride 2
            (void*)data, 1, 2    // right chan offset 1, stride 2
            );

        const qint64 bytesWritten = framesAvailable * 4;
        return bytesWritten;
    }

    // Not used
    qint64 writeData(const char* /*data*/, qint64 /*len*/) override
    {
        return 0;
    }

private:
    fluid_settings_t* m_settings = nullptr;
    fluid_synth_t*    m_synth    = nullptr;
};

