// SPDX-License-Identifier: GPL-3.0-only
#include "grand_staff.hpp"
#include "chord.hpp"
#include "config.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <map>

std::map<Key, int> gKeySharpsFlats = {
    {Key::CMajor, 0},      {Key::GMajor, 1},       {Key::DMajor, 2},      {Key::AMajor, 3},
    {Key::EMajor, 4},      {Key::BMajor, 5},       {Key::FSharpMajor, 6}, {Key::CSharpMajor, 7},
    {Key::FMajor, -1},     {Key::BbMajor, -2},     {Key::EbMajor, -3},    {Key::AbMajor, -4},
    {Key::DbMajor, -5},    {Key::GbMajor, -6},     {Key::CbMajor, -7},    {Key::DSharpMajor, 6},
    {Key::GSharpMajor, 8}, {Key::ASharpMajor, 10},
};

#include <iostream>

void GrandStaff::midiToLetterOctave(int midiNote, int& letter, int& octave) const {
    int h = (midiNote - 60) % 12;
    if (h < 0)
        h += 12;

    static int mapSharps[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};

    static int mapFlats[12] = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};

    if (is_sharp_key(currentKey))
        letter = mapSharps[h];
    else
        letter = mapFlats[h];

    octave = (midiNote / 12) - 1;
}

int GrandStaff::stepsFromRef(int midiNote, int ref) const {
    int l1, o1, l2, o2;
    midiToLetterOctave(midiNote, l1, o1);
    midiToLetterOctave(ref, l2, o2);
    return (l1 + 7 * o1) - (l2 + 7 * o2);
}

GrandStaff::GrandStaff(sf::RenderWindow& window, sf::Font& font) : window(window), font(font) {}

void GrandStaff::setKey(Key key) { currentKey = key; }

void GrandStaff::updateNotes(const std::vector<size_t>& pressedMidiNotes) {
    staffLeftX = window.getSize().x / 10.f;
    staffTopY = window.getSize().y / 4.f;
    noteRadius = window.getSize().x / 200.f;
    staffSpacing = 2 * noteRadius;
    gapBetweenStaves = 2 * staffSpacing;

    std::vector<int> notes;
    notes.reserve(pressedMidiNotes.size());
    for (auto n : pressedMidiNotes) {
        notes.push_back(static_cast<int>(n + 21));
    }
    displayedNotes = notes;
}

void GrandStaff::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    drawStaff(target);
    drawKeySignature(target);
    drawNotes(target);
}

void GrandStaff::drawStaff(sf::RenderTarget& target) const {
    sf::Color c = sf::Color::White;
    float lineLength = target.getView().getSize().x / 10.f;

    for (int i = 0; i < 5; i++) {
        sf::RectangleShape line;
        line.setSize(sf::Vector2f(lineLength, 1.f));
        line.setFillColor(c);
        float y = staffTopY + i * staffSpacing;
        line.setPosition({staffLeftX, y});
        target.draw(line);
    }
    float bassTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;
    for (int i = 0; i < 5; i++) {
        sf::RectangleShape line;
        line.setSize(sf::Vector2f(lineLength, 1.f));
        line.setFillColor(c);
        float y = bassTop + i * staffSpacing;
        line.setPosition({staffLeftX, y});
        target.draw(line);
    }
    sf::RectangleShape brace(sf::Vector2f(5.f, bassTop - staffTopY + 4 * staffSpacing));
    brace.setFillColor(c);
    brace.setPosition({staffLeftX - 5.f, staffTopY});
    target.draw(brace);

    static bool loaded = false;
    static sf::Texture trebleTex;
    static sf::Texture bassTex;
    static sf::Sprite trebleSprite(trebleTex);
    static sf::Sprite bassSprite(bassTex);

    if (!loaded) {
        if (!trebleTex.loadFromFile(std::string(APP_ASSETS_PATH) + "/images/treble_clef.png")) {
            std::cerr << "Failed to load treble_clef.png" << std::endl;
        }
        if (!bassTex.loadFromFile(std::string(APP_ASSETS_PATH) + "/images/bass_clef.png")) {
            std::cerr << "Failed to load bass_clef.png" << std::endl;
        }
        trebleSprite = sf::Sprite(trebleTex);
        bassSprite = sf::Sprite(bassTex);

        float scaleFactor = (5 * staffSpacing) / trebleTex.getSize().y;
        trebleSprite.setScale({scaleFactor, scaleFactor});
        bassSprite.setScale({scaleFactor, scaleFactor});

        loaded = true;
    }

    trebleSprite.setPosition(
        {staffLeftX - 10.f - trebleSprite.getGlobalBounds().size.x, staffTopY});
    target.draw(trebleSprite);

    bassSprite.setPosition({staffLeftX - 10.f - bassSprite.getGlobalBounds().size.x, bassTop});
    target.draw(bassSprite);
}

void GrandStaff::drawKeySignature(sf::RenderTarget& target) const {
    int count = gKeySharpsFlats.at(currentKey);
    if (count == 0)
        return;

    unsigned fontSize = static_cast<unsigned>(staffSpacing * 1.5f);
    auto text = sf::Text(font, "", fontSize);
    text.setFillColor(sf::Color::White);

    static float trebleSharpY[7] = {
        1.f, // F#
        4.f, // C#
        0.f, // G#
        3.f, // D#
        6.f, // A#
        2.f, // E#
        5.f  // B#
    };

    static float trebleFlatY[7] = {
        5.f, // Bb
        2.f, // Eb
        6.f, // Ab
        3.f, // Db
        7.f, // Gb
        4.f, // Cb
        8.f  // Fb
    };

    float trebleX = staffLeftX;
    float trebleBase = staffTopY - fontSize;
    float bassTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;
    float bassX = staffLeftX;
    float bassBase = bassTop - fontSize;

    int howMany = std::abs(count);
    if (count > 0) {
        sf::String sharps = L"♯";
        for (int i = 0; i < howMany && i < 7; i++) {
            text.setString(sharps);
            // Treble staff
            float ty = trebleBase + trebleSharpY[i] * 0.5f * staffSpacing;
            text.setPosition({trebleX + i * fontSize * 0.25f, ty});
            target.draw(text);
            // Bass staff
            float by = bassBase + (trebleSharpY[i] + 2) * 0.5f * staffSpacing;
            text.setPosition({bassX + i * fontSize * 0.25f, by});
            target.draw(text);
        }
    } else {
        sf::String flats = L"♭";
        for (int i = 0; i < howMany && i < 7; i++) {
            text.setString(flats);
            // Treble staff
            float ty = trebleBase + trebleFlatY[i] * 0.5f * staffSpacing;
            text.setPosition({trebleX + i * fontSize * 0.25f, ty});
            target.draw(text);
            // Bass staff
            float by = bassBase + trebleFlatY[i] * 0.5f * staffSpacing;
            text.setPosition({bassX + i * fontSize * 0.25f, by});
            target.draw(text);
        }
    }
}

void GrandStaff::drawNotes(sf::RenderTarget& target) const {
    for (auto midiNote : displayedNotes) {
        float y = noteYPosition(midiNote);
        float x = staffLeftX + target.getView().getSize().x / 20.f;

        sf::CircleShape noteHead(noteRadius);
        noteHead.setFillColor(sf::Color::White);
        noteHead.setPosition({x - noteRadius, y - noteRadius * 0.75f});
        noteHead.setScale({1.f, 0.75f}); // ovalize
        target.draw(noteHead);

        sf::String acc = getAccidentalGlyph(midiNote);
        if (!acc.isEmpty()) {
            sf::Text accidental(font, acc, static_cast<unsigned>(staffSpacing * 1.5f));
            accidental.setFillColor(sf::Color::White);
            accidental.setPosition({x - (staffSpacing * 1.5f), y - (staffSpacing * 0.75f)});
            target.draw(accidental);
        }
    }
}

float GrandStaff::noteYPosition(int midiNote) const {
    bool isBassClef = (midiNote < 60);
    int referenceNote = isBassClef ? 43 : 64;
    int stepCount = stepsFromRef(midiNote, referenceNote);

    float bassStaffTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;
    float trebleStaffBottom = staffTopY + 4 * staffSpacing;
    float bassStaffBottom = bassStaffTop + 4 * staffSpacing;
    float verticalOffset = stepCount * (staffSpacing / 2.f);

    return isBassClef ? (bassStaffBottom - verticalOffset) : (trebleStaffBottom - verticalOffset);
}

bool GrandStaff::isNatural(int midiNote) const {
    static int letterMap[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
    static int sharpLetters[7] = {3, 0, 4, 1, 5, 2, 6};
    static int flatLetters[7] = {6, 2, 5, 1, 4, 0, 3};
    static int baseSemitones[7] = {0, 2, 4, 5, 7, 9, 11};

    int keySignature = gKeySharpsFlats.at(currentKey);
    int letterIndex = letterMap[(midiNote % 12 + 12) % 12];
    int acc = 0;

    if (keySignature > 0) {
        int numAccidentals = (keySignature > 7 ? 7 : keySignature);
        for (int i = 0; i < numAccidentals; i++) {
            if (sharpLetters[i] == letterIndex) {
                acc = 1;
                break;
            }
        }
    } else if (keySignature < 0) {
        int numAccidentals = (-keySignature > 7 ? 7 : -keySignature);
        for (int i = 0; i < numAccidentals; i++) {
            if (flatLetters[i] == letterIndex) {
                acc = -1;
                break;
            }
        }
    }

    int staffPitch = (baseSemitones[letterIndex] + acc + 12) % 12;
    int notePitch = (midiNote - 60 + 12 * 999) % 12;
    int diff = (notePitch - staffPitch + 12) % 12;

    if ((acc == 1 && diff == 11) || (acc == -1 && diff == 1) || (acc == 0 && diff == 0))
        return true;

    return false;
}

sf::String GrandStaff::getAccidentalGlyph(int midiNote) const {
    static int letterMap[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
    static int sharpLetters[7] = {3, 0, 4, 1, 5, 2, 6};
    static int flatLetters[7] = {6, 2, 5, 1, 4, 0, 3};
    static int baseSemitones[7] = {0, 2, 4, 5, 7, 9, 11};

    int keySignature = gKeySharpsFlats.at(currentKey);
    int letterIndex = letterMap[(midiNote % 12 + 12) % 12];
    int acc = 0;

    if (keySignature > 0) {
        int numAccidentals = (keySignature > 7 ? 7 : keySignature);
        for (int i = 0; i < numAccidentals; i++) {
            if (sharpLetters[i] == letterIndex) {
                acc = 1;
                break;
            }
        }
    } else if (keySignature < 0) {
        int numAccidentals = (-keySignature > 7 ? 7 : -keySignature);
        for (int i = 0; i < numAccidentals; i++) {
            if (flatLetters[i] == letterIndex) {
                acc = -1;
                break;
            }
        }
    }

    int staffPitch = (baseSemitones[letterIndex] + acc + 12) % 12;
    int notePitch = (midiNote - 60 + 12 * 999) % 12;

    if (staffPitch == notePitch)
        return sf::String();

    if (isNatural(midiNote))
        return L"♮";
    else {
        if (is_sharp_key(currentKey))
            return L"♯";
        else
            return L"♭";
    }
}
