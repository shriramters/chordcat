#pragma once

#include <QObject>
#include <QVector>
#include <QAudioSink>
#include <QAudioFormat>
#include <QMediaDevices>
#include <fluidsynth.h>
#include <vector>
#include "midi_synth_io_device.hpp"
#include "midi_event.hpp"

/**
 * @brief A Qt-based Piano class, similar in interface to your SFML Piano.
 *        Uses FluidSynth to synthesize audio, streamed via QAudioSink.
 */
class Piano : public QObject
{
    Q_OBJECT

public:

    explicit Piano(QObject* parent = nullptr);
    ~Piano() override;

    /**
     * @brief keyOn Turn on a MIDI note (like SFML Piano::keyOn).
     * @param midi_note_number E.g. 60 for middle C.
     * @param chan MIDI channel to use.
     * @param velocity Velocity (0..127).
     */
    Q_INVOKABLE void keyOn(int midi_note_number, int chan, int velocity);

    /**
     * @brief keyOff Turn off a MIDI note (like SFML Piano::keyOff).
     * @param midi_note_number E.g. 60 for middle C.
     * @param chan MIDI channel to use.
     */
    Q_INVOKABLE void keyOff(int midi_note_number, int chan);

    /**
     * @brief keyToggle Toggle a note on/off.
     *        If it’s off, turn it on at velocity=100; if on, turn it off.
     * @param midi_note_number
     */
    Q_INVOKABLE void keyToggle(int midi_note_number);

    /**
     * @brief clearAllKeys Turn off all notes and reset pressed keys.
     */
    Q_INVOKABLE void clearAllKeys();

    /**
     * @brief getPressedNotes Return indices (0..87) of pressed notes,
     *        where index=0 => MIDI note 21 (A0).
     */
    Q_INVOKABLE std::vector<size_t> getPressedNotes() const;

    /**
     * @brief isNotePressed Return true if note is currently "ON". False otherwise.
     */
    Q_INVOKABLE bool isNotePressed(int midiNote) const;

    /**
     * @brief getSynth Return pointer to the underlying FluidSynth
     *        if you need direct calls (like set reverb, load SoundFonts, etc.).
     */
    Q_INVOKABLE fluid_synth_t* getSynth() const;

    /**
     * @brief midiEvent Handle a generic MIDI event (like your SFML Piano).
     *        NoteOn => keyOn, NoteOff => keyOff, etc.
     */
    Q_INVOKABLE void midiEvent(const MidiEvent& me);

    /**
     * @brief getChannel Return the current default channel.
     *        (If you want one global channel, you can use this.)
     */
    Q_INVOKABLE int getChannel() const;

    /**
     * @brief setChannel Sets the default channel used if you want
     *        to call keyOn/keyOff ignoring their channel param (or for program changes).
     */
    Q_INVOKABLE void setChannel(int chan);

    Q_INVOKABLE void programChange(int chan, int program);

    Q_INVOKABLE void setGain(qreal gain);
    Q_INVOKABLE void loadSoundFont(const QString& path);

    Q_INVOKABLE QVector<int> getNotePressingChannels(int midiNote) const;

private:
    // Low-level “turn note on/off” that manipulates m_pressedKeys & FluidSynth
    void keyOnInternal(int midi_note_number, int chan, int velocity);
    void keyOffInternal(int midi_note_number, int chan);

private:
    // QAudio + FluidSynth objects
    MidiSynthIODevice* m_synthDevice = nullptr;
    QAudioSink*        m_audioSink   = nullptr;
    fluid_synth_t*     m_synth       = nullptr;

    // Pressed state for each note in [A0..C8] => [21..108] for all 16 channels
    QVector<QVector<bool>> m_pressedKeys;

    int m_channel = 0;

signals:
    void noteChannelsChanged(int midiNote, const QVector<int>& channels);
    void pressedNotesChanged(const std::vector<size_t>& pressedNotes);
};
