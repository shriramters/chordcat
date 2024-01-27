#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>

class Piano : public sf::Drawable {
public:
    Piano();
    void setKeyPressed(size_t midi_note_number, bool isPressed);
    std::vector<size_t> getPressedNotes();
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states)const;
    std::array<bool, 88> keys = {};
};