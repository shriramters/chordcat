#include "looper.hpp"
#include "midi_event.hpp"
#include <iostream>
#include <set>

Looper::Looper(Piano& piano, Metronome& metronome) : metronome(metronome), piano(piano) {
    using namespace std::placeholders;
    piano.setMidiEventCallback(std::bind(&Looper::recordEvent, this, _1));
}

void Looper::startRecording() {
    events.clear();
    metronome.start();
    state = LooperState::CountingIn;
}

void Looper::stopRecording() {
    if (state == LooperState::Recording) {
        state = LooperState::CountingOut;
    }
}

void Looper::recordEvent(MidiEvent me) {
    if (state == LooperState::Recording || state == LooperState::Overdubbing) {
        me.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        state == LooperState::Recording ? events.push_back(me) : overdubEvents.push_back(me);
    }
}

void Looper::startPlayback() {
    state = LooperState::PlayingBack;
    startTime = std::chrono::steady_clock::now();
}

void Looper::startOverdub() { state = LooperState::CountingInToOverdub; }

void Looper::stopOverdub() { state = LooperState::PlayingBack; }

void Looper::stopPlayback() { state = LooperState::Idle; }

void Looper::update() {
    if (state == LooperState::CountingIn) {
        if (metronome.getBeat() >= 4) {
            startTime = std::chrono::steady_clock::now();
            state = LooperState::Recording;
        }
    } else if (state == LooperState::Recording) {
        if (metronome.getBeat() - 4 == numBars * 4) {
            loopLength = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime);
            startPlayback();
        }
    } else if (state == LooperState::CountingOut) {
        if (metronome.getBeat() % 4 == 0) {
            state = LooperState::PlayingBack;
            loopLength = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime);
        }
    } else if (state == LooperState::PlayingBack || state == LooperState::CountingInToOverdub ||
               state == LooperState::Overdubbing) {
        static std::set<MidiEvent> played{};
        auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        if (currentTime >= loopLength) {
            startTime = std::chrono::steady_clock::now();
            played.clear();
            if (state == LooperState::CountingInToOverdub)
                state = LooperState::Overdubbing;
            else if (state == LooperState::Overdubbing) {
                state = LooperState::PlayingBack;
                events.insert(events.end(), overdubEvents.begin(), overdubEvents.end());
                overdubEvents.clear();
            }
            currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime);
        }
        for (const auto& event : events) {
            if (currentTime >= event.timestamp && !played.contains(event)) {
                played.insert(event);
                piano.midiEvent(event);
            }
        }
    }
}

LooperState Looper::getState() { return this->state; }

void Looper::setBars(unsigned int _numBars) { numBars = _numBars; };

int Looper::getBars() { return numBars; }
