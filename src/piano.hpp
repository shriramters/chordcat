// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "midi_audio_stream.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <fluidsynth.h>
#include <vector>

class Piano : public sf::Drawable {
public:
    Piano(sf::RenderWindow& window, std::array<float, 4>& pressed_note_colors);
    void keyOn(int midi_note_number, int velocity);
    void keyOff(int midi_note_number);
    void keyToggle(int midi_note_number);   
    void clearAllKeys();
    std::vector<size_t> getPressedNotes();
    fluid_synth_t* getSynth();
    void processEvent(sf::Event& event);
    float key_aspect_ratio = 4.f;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void mouseEvent(sf::Event& event);
    void keyboardEvent(sf::Event& event);
    std::array<bool, 88> keys = {};
    mutable std::array<sf::RectangleShape, 88> key_sprites;
    sf::RenderWindow& window;
    MidiAudioStream mas{};
    fluid_synth_t* synth = nullptr;
    std::array<float, 4>& note_colors;
};
