#pragma once

#include <chrono>
#include <fluidsynth.h>
#include <fluidsynth/synth.h>
#include <thread>

// looks like fluid_synth api is thread safe by default, so not using a mutex

class Metronome {
  public:
    Metronome(fluid_synth_t* synth, int bpm, int default_soundfont_id)
        : synth(synth), bpm(bpm), isRunning(false), beat(0),
          default_soundfont_id(default_soundfont_id) {}

    void start() {
        beat = 0;
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

    unsigned long long getBeat() { return beat; }

    ~Metronome() { stop(); }

  private:
    fluid_synth_t* synth;
    int bpm;
    bool isRunning;
    unsigned long long beat = 0;
    std::thread metronomeThread;
    int default_soundfont_id = -1;

    void run() {
        while (isRunning) {
            int interval = 60000 / bpm;
            int note = (beat % 4 == 0) ? 76 : 77; // E5 for downbeat, F5 for others

            fluid_synth_noteon(synth, 9, note, 100); // Channel 9, GM Percussion
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            fluid_synth_noteoff(synth, 9, note);

            std::this_thread::sleep_for(std::chrono::milliseconds(interval - 50));
            beat++;
        }
    }
};
