#pragma once

#include "metronome.hpp"
#include "midi_event.hpp"
#include "piano.hpp"
#include <chrono>
#include <libremidi/libremidi.hpp>
#include <vector>

enum class LooperState {
    Idle,
    CountingIn,
    Recording,
    CountingOut,
    PlayingBack,
    CountingInToOverdub,
    Overdubbing
};

class Looper {
  public:
    void startRecording();
    void stopRecording();
    void startPlayback();
    void stopPlayback();
    void startOverdub();
    void stopOverdub();
    void cancelOverdub();
    void setBars(unsigned int numBars);
    int getBars();
    void recordEvent(MidiEvent me);
    void update();
    LooperState getState();
    Looper(Piano& piano, Metronome& metronome);

  private:
    Piano& piano;
    Metronome& metronome;
    LooperState state = LooperState::Idle;
    unsigned int numBars = 4;

    std::vector<MidiEvent> events;
    std::vector<MidiEvent> overdubEvents;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::milliseconds loopLength;
};
