#pragma once
#include "libremidi/message.hpp"
#include <chrono>

enum class MidiMessageType {
    NoteOff = 0x8,
    NoteOn = 0x9,
    AfterTouch = 0xA,
    CC = 0xB,
    ProgramChange = 0xC,
    ChanPressure = 0xD,
    PitchWheel = 0xE,
    System = 0xF
};

struct MidiEvent {
    MidiMessageType messageType;
    int chan;
    int data0;
    int data1;
    std::chrono::milliseconds timestamp;
    auto operator<=>(const MidiEvent& me) const = default;

    MidiEvent(const libremidi::message& message) {
        messageType = static_cast<MidiMessageType>(message[0] >> 4);
        chan = static_cast<int>(message[0] & 0b1111);
        data0 = message[1];
        data1 = message[2];
    }

    MidiEvent(MidiMessageType mt, int chan, int data0, int data1)
        : messageType(mt), chan(chan), data0(data0), data1(data1) {}
};
