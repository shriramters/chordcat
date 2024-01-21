#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

class Piano {
public:
    Piano(sf::RenderWindow& window);
    void Draw();
    void setKeyPressed(size_t midi_note_number, bool isPressed);
    std::vector<size_t> getPressedNotes();
private:
    sf::RenderWindow& window;
    std::array<bool, 88> keys = {};
    sf::Color getKeyColor(size_t index);
};