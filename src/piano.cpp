// SPDX-License-Identifier: GPL-3.0-only
#include "piano.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <fluidsynth/synth.h>
#include <cstdint>

inline int getNoteFromKeyCode(sf::Keyboard::Key keycode) {
    int index = 21 + 3; // 21 is A0, 21 + 3 is C1
    
    switch(keycode) {
    case sf::Keyboard::Z:    index+=0; break;
    case sf::Keyboard::S:    index+=1; break;
    case sf::Keyboard::X:    index+=2; break;
    case sf::Keyboard::D:    index+=3; break;
    case sf::Keyboard::C:    index+=4; break;
    case sf::Keyboard::V:    index+=5; break;
    case sf::Keyboard::G:    index+=6; break;
    case sf::Keyboard::B:    index+=7; break;
    case sf::Keyboard::H:    index+=8; break;
    case sf::Keyboard::N:    index+=9; break;
    case sf::Keyboard::J:    index+=10; break;
    case sf::Keyboard::M:    index+=11; break;         
    case sf::Keyboard::Q:    index+=12; break;
    case sf::Keyboard::Num2: index+=13; break;
    case sf::Keyboard::W:    index+=14; break;
    case sf::Keyboard::Num3: index+=15; break;
    case sf::Keyboard::E:    index+=16; break;
    case sf::Keyboard::R:    index+=17; break;
    case sf::Keyboard::Num5: index+=18; break;
    case sf::Keyboard::T:    index+=19; break;
    case sf::Keyboard::Num6: index+=20; break;
    case sf::Keyboard::Y:    index+=21; break;
    case sf::Keyboard::Num7: index+=22; break;
    case sf::Keyboard::U:    index+=23; break;        
    case sf::Keyboard::I:    index+=24; break;
    case sf::Keyboard::Num9: index+=25; break;
    case sf::Keyboard::O:    index+=26; break;
    case sf::Keyboard::Num0: index+=27; break;
    case sf::Keyboard::P:    index+=28; break;
    default: return -1;
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

Piano::Piano(sf::RenderWindow &window, std::array<float, 4> &pressed_note_colors)
    : window(window), note_colors(pressed_note_colors) {
    synth = mas.getSynth();
    mas.play();
}

fluid_synth_t* Piano::getSynth() {
    return synth;
}

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
    key_sprites[0].setPosition(0, ypos - key_height_white);
    key_sprites[0].setFillColor(getKeyColor(0));
    size_t last_white_key_index = 0;
    // White Keys
    for (size_t i = 1; i < 88; i++) {
        if (!isBlackKey(i)) {
            key_sprites[i] = sf::RectangleShape(sf::Vector2f(key_width_white, key_height_white));
            key_sprites[i].setPosition(key_sprites[last_white_key_index].getPosition().x +
                                           key_width_white,
                                       ypos - key_height_white);
            key_sprites[i].setFillColor(getKeyColor(i));
            key_sprites[i].setOutlineColor(sf::Color::Black);
            key_sprites[i].setOutlineThickness(key_width_white / 10.f);
            last_white_key_index = i;
        } else {
            key_sprites[i] = sf::RectangleShape(sf::Vector2f(key_width_black, key_height_black));
            key_sprites[i].setPosition(key_sprites[last_white_key_index].getPosition().x +
                                           key_width_white - key_width_black / 2.f,
                                       ypos - key_height_white);
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

void Piano::keyOn(int midi_note, int velocity) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0 && !keys[index]) {
        fluid_synth_noteon(synth, 0, midi_note, velocity);
        keys[index] = true;
    }
}

void Piano::keyOff(int midi_note) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0 && keys[index]) {
        fluid_synth_noteoff(synth, 0, midi_note);
        keys[index] = false;
    }
}

void Piano::keyToggle(int midi_note) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0) {
        keys[index] ?
            fluid_synth_noteoff(synth, 0, midi_note):
            fluid_synth_noteon(synth, 0, midi_note, 100);
        keys[index] = !keys[index];
    }
}

std::vector<size_t> Piano::getPressedNotes() {
    std::vector<size_t> pressed_notes = {};
    for (int i = 0; i < keys.size(); i++) {
        if (keys[i])
            pressed_notes.push_back(i);
    }
    return pressed_notes;
}

void Piano::processEvent(sf::Event &event) {
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
        if(event.key.code == sf::Keyboard::Hyphen) {
            if(octave >= 0)
                octave--;
        }
        if(event.key.code == sf::Keyboard::Equal) {
            if(octave <= 6)
                octave++;
        }
        int note = getNoteFromKeyCode(event.key.code);
        if(note < 0) return;
        keyOn(note + octave * 12, 100);
    }
    
    if (event.type == sf::Event::KeyReleased) {
        int note = getNoteFromKeyCode(event.key.code);
        if(note < 0) return;
        keyOff(note + octave * 12);
    }
}

void Piano::clearAllKeys() { keys.fill(false); }
