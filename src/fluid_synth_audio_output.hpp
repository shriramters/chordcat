#pragma once

#include <QString>
#include <condition_variable>
#include <deque>
#include <fluidsynth.h>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>

class FluidSynthAudioOutput
{
public:
    FluidSynthAudioOutput();
    ~FluidSynthAudioOutput();

    fluid_synth_t* synth() const;

    void setGain(qreal gain) const;
    bool loadSoundFont(const QString& path) const;
    void noteOn(int chan, int midiNote, int velocity) const;
    void noteOff(int chan, int midiNote) const;
    void cc(int chan, int control, int value) const;
    void programChange(int chan, int program) const;
    void pitchBend(int chan, int value) const;
    void allNotesOff() const;

private:
    using SynthCommand = std::function<void(fluid_synth_t*)>;

    template <typename Fn>
    auto call(Fn&& fn) const -> decltype(fn(static_cast<fluid_synth_t*>(nullptr)));

    void run();

    mutable std::mutex m_mutex;
    mutable std::condition_variable m_condition;
    mutable std::deque<SynthCommand> m_pendingCommands;
    std::thread m_thread;
    fluid_synth_t* m_synth = nullptr;
    bool m_running = true;
    bool m_ready = false;
};

template <typename Fn>
auto FluidSynthAudioOutput::call(Fn&& fn) const -> decltype(fn(static_cast<fluid_synth_t*>(nullptr)))
{
    using Result = decltype(fn(static_cast<fluid_synth_t*>(nullptr)));

    auto promise = std::make_shared<std::promise<Result>>();
    auto future = promise->get_future();

    {
        std::lock_guard lock(m_mutex);
        m_pendingCommands.emplace_back(
            [promise, func = std::forward<Fn>(fn)](fluid_synth_t* synth) mutable {
                if constexpr (std::is_void_v<Result>) {
                    func(synth);
                    promise->set_value();
                } else {
                    promise->set_value(func(synth));
                }
            });
    }

    m_condition.notify_one();
    if constexpr (std::is_void_v<Result>) {
        future.get();
    } else {
        return future.get();
    }
}
