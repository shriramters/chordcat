#pragma once
#include "chord.hpp"
#include <SFML/Graphics.hpp>

class GrandStaff : public sf::Drawable {
  public:
    GrandStaff(sf::RenderWindow& window, sf::Font& font);
    void setKey(Key key);
    void updateNotes(const std::vector<size_t>& pressedMidiNotes);
    void setNotes(const std::vector<int>& midiNotes);

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void drawStaff(sf::RenderTarget& target) const;
    void drawKeySignature(sf::RenderTarget& target) const;
    void drawNotes(sf::RenderTarget& target) const;
    float noteYPosition(int midiNote) const;
    bool isNatural(int midiNote) const;
    sf::String getAccidentalGlyph(int midiNote) const;
    int stepsFromRef(int midiNote, int ref) const;
    void midiToLetterOctave(int midiNote, int& letter, int& octave) const;

  private:
    Key currentKey = Key::CMajor;
    std::vector<int> displayedNotes;
    float staffLeftX;
    float staffTopY;
    float staffSpacing;
    float noteRadius;
    float gapBetweenStaves;
    sf::Font& font;
    sf::RenderWindow& window;
};
