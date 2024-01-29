// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <fluidsynth.h>
#include <vector>

class Piano : public sf::Drawable {
public:
    Piano();
    void setKeyPressed(size_t midi_note_number, bool isPressed);
    std::vector<size_t> getPressedNotes();
    void mouseEvent(sf::Event &event, sf::RenderWindow &window, fluid_synth_t *synth);

private:
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
    std::array<bool, 88> keys = {};
    mutable std::array<sf::RectangleShape, 88> key_sprites;
};
