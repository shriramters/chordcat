#pragma once

#include <chrono>
#include <fluidsynth.h>
#include <thread>

// looks like fluid_synth api is thread safe by default, so not using a mutex

class Metronome {
  public:
    Metronome(fluid_synth_t* synth, int bpm) : synth(synth), bpm(bpm), isRunning(false), beat(0) {}

    void start() {
        isRunning = true;
        metronomeThread = std::thread(&Metronome::run, this);
    }

    void stop() {
        isRunning = false;
        if (metronomeThread.joinable()) {
            metronomeThread.join();
        }
    }

    void toggle() {
        if (isRunning) {
            stop();
        } else
            start();
    }

    void setBpm(int newBpm) { bpm = newBpm; }

    ~Metronome() { stop(); }

  private:
    fluid_synth_t* synth;
    int bpm;
    bool isRunning;
    int beat;
    std::thread metronomeThread;

    void run() {
        while (isRunning) {
            int interval = 60000 / bpm;
            int note = (beat % 4 == 0) ? 76 : 77; // E5 for downbeat, F5 for others

            fluid_synth_noteon(synth, 9, note, 100); // Channel 9, GM Percussion
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            fluid_synth_noteoff(synth, 9, note);

            beat++;
            std::this_thread::sleep_for(std::chrono::milliseconds(interval - 50));
        }
    }
};
