// SPDX-License-Identifier: GPL-3.0-only
#include "piano.hpp"

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

Piano::Piano() {}

void Piano::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    float key_width_white = target.getView().getSize().x / 52;
    float key_height_white = key_width_white * key_aspect_ratio;
    float key_width_black = key_width_white / 2;
    float key_height_black = key_width_black * key_aspect_ratio;
    unsigned ypos = target.getView().getSize().y;

    auto getKeyColor = [this](size_t index) {
        if (this->keys[index])
            return sf::Color(static_cast<uint8_t>(note_colors[0] * 255), static_cast<uint8_t>(note_colors[1] * 255), static_cast<uint8_t>(note_colors[2] * 255), static_cast<uint8_t>(note_colors[3] * 255));
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
        }
        else {
            key_sprites[i] = sf::RectangleShape(sf::Vector2f(key_width_black, key_height_black));
            key_sprites[i].setPosition(key_sprites[last_white_key_index].getPosition().x +
                key_width_white - key_width_black / 2.f,
                ypos - key_height_white);
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

void Piano::setKeyPressed(size_t midi_note, bool isPressed) {
    size_t index = midi_note - 21; // 21 is A0;
    if (index < keys.size() && index >= 0) {
        keys[index] = isPressed;
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

void Piano::mouseEvent(sf::Event& event, sf::RenderWindow& window, fluid_synth_t* synth) {
    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
                if (isBlackKey(std::distance(key_sprites.begin(), it)))
                    if (it->getGlobalBounds().contains(worldPos)) {
                        size_t index = std::distance(key_sprites.begin(), it);
                        if (keys[index]) {
                            fluid_synth_noteoff(synth, 0, (int)(index + 21));
                        }
                        else {
                            fluid_synth_noteon(synth, 0, (int)(index + 21), 100);
                        }
                        keys[index] = !keys[index];
                        return;
                    }
            }
            for (auto it = key_sprites.begin(); it != key_sprites.end(); it++) {
                if (it->getGlobalBounds().contains(worldPos)) {
                    size_t index = std::distance(key_sprites.begin(), it);
                    if (keys[index]) {
                        fluid_synth_noteoff(synth, 0, (int)(index + 21));
                    }
                    else {
                        fluid_synth_noteon(synth, 0, (int)(index + 21), 100);
                    }
                    keys[index] = !keys[index];
                    return;
                }
            }
        }
    }
}

void Piano::clearAllKeys() {
    keys.fill(false);
}