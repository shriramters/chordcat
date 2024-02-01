// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <SFML/Audio.hpp>
#include <fluidsynth.h>
#include <vector>

class MidiAudioStream : public sf::SoundStream {
public:
    MidiAudioStream() {
        settings = new_fluid_settings();
        synth = new_fluid_synth(settings);
        m_samples.resize(audioBufferSize);
        // reset the current playing position
        m_currentSample = 0;
        // initialize the base class
        initialize(2, 44100);
    }
    void load(const sf::SoundBuffer& buffer) {
        // extract the audio samples from the sound buffer to our own container
        m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

        // reset the current playing position
        m_currentSample = 0;

        // initialize the base class
        initialize(2, 44100);
    }

    fluid_synth_t* getSynth() { return synth; }

    ~MidiAudioStream() {
        stop();
        delete_fluid_synth(synth);
        delete_fluid_settings(settings);
    }

private:
    virtual bool onGetData(Chunk& data) {
        int read =
            fluid_synth_write_s16(synth, samplesToStream, &m_samples[0], 0, 2, &m_samples[0], 1, 2);

        data.sampleCount = audioBufferSize;
        data.samples = &m_samples[0];
        return true;
    }

    virtual void onSeek(sf::Time timeOffset) {
        // compute the corresponding sample index according to the sample rate and channel count
        m_currentSample =
            static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }

    fluid_settings_t* settings;
    fluid_synth_t* synth;
    std::vector<sf::Int16> m_samples;
    std::size_t m_currentSample;
    static const unsigned int samplesToStream = 512;
    static const unsigned int audioBufferSize = samplesToStream * 2;
};
