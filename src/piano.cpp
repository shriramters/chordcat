// SPDX-License-Identifier: GPL-3.0-only
#include "piano.hpp"
#include "midi_event.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <cstdint>
#include <fluidsynth/synth.h>

inline int getNoteFromKeyCode(sf::Keyboard::Key keycode) {
    int index = 21 + 3; // 21 is A0, 21 + 3 is C1

    switch (keycode) {
    case sf::Keyboard::Key::Z:
        index += 0;
        break;
    case sf::Keyboard::Key::S:
        index += 1;
        break;
    case sf::Keyboard::Key::X:
        index += 2;
        break;
    case sf::Keyboard::Key::D:
        index += 3;
        break;
    case sf::Keyboard::Key::C:
        index += 4;
        break;
    case sf::Keyboard::Key::V:
        index += 5;
        break;
    case sf::Keyboard::Key::G:
        index += 6;
        break;
    case sf::Keyboard::Key::B:
        index += 7;
        break;
    case sf::Keyboard::Key::H:
        index += 8;
        break;
    case sf::Keyboard::Key::N:
        index += 9;
        break;
    case sf::Keyboard::Key::J:
        index += 10;
        break;
    case sf::Keyboard::Key::M:
        index += 11;
        break;
    case sf::Keyboard::Key::Q:
        index += 12;
        break;
    case sf::Keyboard::Key::Num2:
        index += 13;
        break;
    case sf::Keyboard::Key::W:
        index += 14;
        break;
    case sf::Keyboard::Key::Num3:
        index += 15;
        break;
    case sf::Keyboard::Key::E:
        index += 16;
        break;
    case sf::Keyboard::Key::R:
        index += 17;
        break;
    case sf::Keyboard::Key::Num5:
        index += 18;
        break;
    case sf::Keyboard::Key::T:
        index += 19;
        break;
    case sf::Keyboard::Key::Num6:
        index += 20;
        break;
    case sf::Keyboard::Key::Y:
        index += 21;
        break;
    case sf::Keyboard::Key::Num7:
        index += 22;
        break;
    case sf::Keyboard::Key::U:
        index += 23;
        break;
    case sf::Keyboard::Key::I:
        index += 24;
        break;
    case sf::Keyboard::Key::Num9:
        index += 25;
        break;
    case sf::Keyboard::Key::O:
        index += 26;
        break;
    case sf::Keyboard::Key::Num0:
        index += 27;
        break;
    case sf::Keyboard::Key::P:
        index += 28;
        break;
    default:
        return -1;
    }
    return index;
}

inline bool isBlackKey(size_t index) {
    switch (index % 12) {
    case 1:
    case 4:
    case 6:
    case 9:
    case 11:
        return true;
    default:
        return false;
    }
}

Piano::Piano(sf::RenderWindow& window, std::array<float, 4>& pressed_note_colors)
    : window(window), note_colors(pressed_note_colors) {
    synth = mas.getSynth();
    mas.play();
}

fluid_synth_t* Piano::getSynth() const { return synth; }

void Piano::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    float key_width_white = target.getView().getSize().x / 52;
    float key_height_white = key_width_white * key_aspect_ratio;
    float key_width_black = key_width_white / 2;
    float key_height_black = key_width_black * key_aspect_ratio;
    unsigned ypos = target.getView().getSize().y;

    auto getKeyColor = [this](size_t index) {
        if (this->keys[index])
            return sf::Color(static_cast<uint8_t>(note_colors[0] * 255),
                             static_cast<uint8_t>(note_colors[1] * 255),
                             static_cast<uint8_t>(note_colors[2] * 255),
                             static_cast<uint8_t>(note_colors[3] * 255));
        else if (isBlackKey(index))
            return sf::Color::Black;
        else
            return sf::Color::White;
    };

    key_sprites[0] = sf::RectangleShape(sf::Vector2f(key_width_white, key_height_white));
    key_sprites[0].setPosition({0, ypos - key_height_white});
    key_sprites[0].setFillColor(getKeyColor(0));
    size_t last_white_key_index = 0;
    // White Keys
    for (size_t i = 1; i < 88; i++) {
        if (!isBlackKey(i)) {
            key_sprites[i] = sf::RectangleShape(sf::Vector2f(key_width_white, key_height_white));
            key_sprites[i].setPosition({key_sprites[last_white_key_index].getPosition().x +
                                           key_width_white,
                                       ypos - key_height_white});
            key_sprites[i].setFillColor(getKeyColor(i));
            key_sprites[i].setOutlineColor(sf::Color::Black);
            key_sprites[i].setOutlineThickness(key_width_white / 10.f);
            last_white_key_index = i;
        } else {
            key_sprites[i] = sf::RectangleShape(sf::Vector2f(key_width_black, key_height_black));
            key_sprites[i].setPosition({key_sprites[last_white_key_index].getPosition().x +
                                           key_width_white - key_width_black / 2.f,
                                       ypos - key_height_white});
            key_sprites[i].setOutlineColor(sf::Color::Black);
            key_sprites[i].setOutlineThickness(key_width_black / 10.f);
            key_sprites[i].setFillColor(getKeyColor(i));
        }
    }
    for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
        if (!isBlackKey(std::distance(key_sprites.begin(), it)))
            target.draw(*it, states);
    }
    for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
        if (isBlackKey(std::distance(key_sprites.begin(), it)))
            target.draw(*it, states);
    }
}

void Piano::keyOn(int midi_note, int chan, int velocity) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0 && !keys[index]) {
        fluid_synth_noteon(synth, chan, midi_note, velocity);
        keys[index] = true;
    }
}

void Piano::keyOff(int midi_note, int chan) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0 && keys[index]) {
        fluid_synth_noteoff(synth, chan, midi_note);
        keys[index] = false;
    }
}

void Piano::keyToggle(int midi_note) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0) {
        keys[index] ? midiEvent({MidiMessageType::NoteOff, channel, midi_note, 100})
                    : midiEvent({MidiMessageType::NoteOn, channel, midi_note, 100});
    }
}

std::vector<size_t> Piano::getPressedNotes() const {
    std::vector<size_t> pressed_notes = {};
    for (int i = 0; i < keys.size(); i++) {
        if (keys[i])
            pressed_notes.push_back(i);
    }
    return pressed_notes;
}

void Piano::processEvent(sf::Event& event) {
    this->mouseEvent(event);
    this->keyboardEvent(event);
}

void Piano::mouseEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
                if (isBlackKey(std::distance(key_sprites.begin(), it)))
                    if (it->getGlobalBounds().contains(worldPos)) {
                        size_t index = std::distance(key_sprites.begin(), it);
                        keyToggle(index + 21);
                        return;
                    }
            }
            for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
                if (it->getGlobalBounds().contains(worldPos)) {
                    size_t index = std::distance(key_sprites.begin(), it);
                    keyToggle(index + 21);
                    return;
                }
            }
        }
    }
}

void Piano::keyboardEvent(sf::Event& event) {
    static int octave = 1;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Hyphen) {
            if (octave >= 0)
                octave--;
        }
        if (event.key.code == sf::Keyboard::Key::Equal) {
            if (octave <= 6)
                octave++;
        }
        int note = getNoteFromKeyCode(event.key.code);
        if (note < 0)
            return;
        midiEvent({MidiMessageType::NoteOn, channel, note + octave * 12, 100});
    }

    if (event.type == sf::Event::KeyReleased) {
        int note = getNoteFromKeyCode(event.key.code);
        if (note < 0)
            return;
        midiEvent({MidiMessageType::NoteOff, channel, note + octave * 12, 0});
    }
}

void Piano::clearAllKeys() { keys.fill(false); }

void Piano::midiEvent(const MidiEvent& me) {
    // 0th byte: status byte containing message type and
    // channel. we only care for the first nibble, see if it
    // is noteOn or noteOff while ignoring the channel 1st
    // byte: note number (for noteOn and noteOff) 2nd byte:
    // velocity (for noteOn and
    switch (me.messageType) {
    case MidiMessageType::NoteOn:
        if (me.data1 > 0) {
            keyOn(me.data0, me.chan, me.data1);
            break;
        }
    case MidiMessageType::NoteOff:
        keyOff(me.data0, me.chan);
        break;
    case MidiMessageType::CC: {
        // control codes such as sustain pedal
        fluid_synth_cc(this->synth, me.chan, me.data0, me.data1);
        break;
    }
    case MidiMessageType::ProgramChange: {
        fluid_synth_program_change(this->synth, me.chan, me.data0);
        break;
    }
    case MidiMessageType::PitchWheel: {
        fluid_synth_pitch_bend(this->synth, me.chan, me.data0);
        break;
    }
    default:
        break;
    }
    midiEventCallback(me);
}

void Piano::setMidiEventCallback(MidiEventCallback callback) { midiEventCallback = callback; }

int Piano::getChannel() const { return channel; }

void Piano::setChannel(int chan) { channel = chan; }
