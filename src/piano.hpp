// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <fluidsynth.h>
#include <vector>

class Piano : public sf::Drawable {
public:
    Piano(std::array<float, 4>& pressed_note_colors);
    void setKeyPressed(size_t midi_note_number, bool isPressed);
    void clearAllKeys();
    std::vector<size_t> getPressedNotes();
    void mouseEvent(sf::Event& event, sf::RenderWindow& window, fluid_synth_t* synth);
    float key_aspect_ratio = 4.f;
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    std::array<bool, 88> keys = {};
    mutable std::array<sf::RectangleShape, 88> key_sprites;
    std::array<float, 4>& note_colors;
};
