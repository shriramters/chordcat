// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "midi_audio_stream.hpp"
#include "midi_event.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <fluidsynth.h>
#include <functional>
#include <vector>

class Piano : public sf::Drawable {
  public:
    using MidiEventCallback = std::function<void(const MidiEvent&)>;
    Piano(sf::RenderWindow& window, std::array<float, 4>& pressed_note_colors);
    void keyOn(int midi_note_number, int chan, int velocity);
    void keyOff(int midi_note_number, int chan);
    void keyToggle(int midi_note_number);
    void clearAllKeys();
    std::vector<size_t> getPressedNotes() const;
    fluid_synth_t* getSynth() const;
    void processEvent(sf::Event& event);
    void midiEvent(const MidiEvent& me);
    void setMidiEventCallback(MidiEventCallback callback);
    float key_aspect_ratio = 4.f;
    int getChannel() const;
    void setChannel(int chan);

  private:
    MidiEventCallback midiEventCallback;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void mouseEvent(sf::Event& event);
    void keyboardEvent(sf::Event& event);
    std::array<bool, 88> keys = {};
    mutable std::array<sf::RectangleShape, 88> key_sprites;
    sf::RenderWindow& window;
    MidiAudioStream mas{};
    fluid_synth_t* synth = nullptr;
    std::array<float, 4>& note_colors;
    int channel = 0;
};
