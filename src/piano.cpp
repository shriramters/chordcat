#include "piano.hpp"
#include <QDebug>

// For an 88-key piano: A0..C8 -> 21..108 -> 88 total
static constexpr int kLowestMIDINote = 21;  // A0
static constexpr int kNumKeys        = 88; // A0..C8

static inline int noteToIndex(int midiNote)
{
    return midiNote - kLowestMIDINote; // 60 => 39
}

bool Piano::isNotePressed(int midiNote) const
{
    int idx = midiNote - kLowestMIDINote; // Convert MIDI note to index (0..87)
    if (idx < 0 || idx >= kNumKeys) {
        return false;
    }
    for (int i = 0; i < 16; ++i) {
        if (m_pressedKeys[i][idx]) {
            return true;
        }
    }
    return false;
}

Piano::Piano(QObject* parent)
    : QObject(parent)
    , m_pressedKeys(16, QVector<bool>(kNumKeys, false))
{
    m_audioOutput = new FluidSynthAudioOutput();
    m_synth = m_audioOutput->synth();

    qDebug() << "Qt-based Piano initialized.";
}

Piano::~Piano()
{
    delete m_audioOutput;
    qDebug() << "Qt-based Piano destroyed.";
}

void Piano::setGain(qreal gain)
{
    m_audioOutput->setGain(gain);
}

void Piano::loadSoundFont(const QString& path)
{
    if (m_audioOutput->loadSoundFont(path)) {
        qDebug() << "SoundFont loaded:" << path;
    } else {
        qWarning() << "Failed to load SoundFont:" << path;
    }
}

void Piano::setChannel(int chan)
{
    m_channel = chan;
}

int Piano::getChannel() const
{
    return m_channel;
}

void Piano::keyOn(int midi_note_number, int chan, int velocity)
{
    keyOnInternal(midi_note_number, chan, velocity);
}

void Piano::keyOff(int midi_note_number, int chan)
{
    keyOffInternal(midi_note_number, chan);
}

void Piano::keyToggle(int midi_note_number)
{
    int idx = noteToIndex(midi_note_number);
    if (idx < 0 || idx >= kNumKeys) return;

    if (m_pressedKeys[m_channel][idx]) {
        keyOff(midi_note_number, m_channel);
    } else {
        keyOn(midi_note_number, m_channel, 100);
    }
}

void Piano::clearAllKeys()
{
    m_audioOutput->allNotesOff();

    for (int c = 0; c < 16; ++c) {
        m_pressedKeys[c].fill(false);
    }
    emit pressedNotesChanged(getPressedNotes());
}

std::vector<size_t> Piano::getPressedNotes() const
{
    std::vector<size_t> pressed;
    pressed.reserve(kNumKeys);
    for (int i = 0; i < kNumKeys; ++i) {
        for (int c = 0; c < 16; ++c) {
            if (m_pressedKeys[c][i]) {
                pressed.push_back(i);
                break; // Don't add the same note index twice
            }
        }
    }
    return pressed;
}

QVector<int> Piano::getNotePressingChannels(int midiNote) const
{
    QVector<int> channels;
    int idx = noteToIndex(midiNote);
    if (idx < 0 || idx >= kNumKeys) {
        return channels;
    }

    for (int c = 0; c < 16; ++c) {
        if (m_pressedKeys[c][idx]) {
            channels.push_back(c);
        }
    }
    return channels;
}

fluid_synth_t* Piano::getSynth() const
{
    return m_synth;
}

void Piano::midiEvent(const MidiEvent& me)
{
    // Mirror logic from your SFML version
    switch (me.messageType) {
    case MidiMessageType::NoteOn:
        if (me.data1 > 0) {
            keyOn(me.data0, me.chan, me.data1);
            break;
        }
        // If velocity=0 => treat as NoteOff
        Q_FALLTHROUGH();
    case MidiMessageType::NoteOff:
        keyOff(me.data0, me.chan);
        break;

    case MidiMessageType::CC:
        m_audioOutput->cc(me.chan, me.data0, me.data1);
        break;

    case MidiMessageType::ProgramChange:
        m_audioOutput->programChange(me.chan, me.data0);
        break;

    case MidiMessageType::PitchWheel:
        m_audioOutput->pitchBend(me.chan, me.data0);
        break;

    default:
        break;
    }
}

// -- Private internal versions that do the actual noteOn/noteOff
void Piano::keyOnInternal(int midi_note_number, int chan, int velocity)
{
    int idx = noteToIndex(midi_note_number);
    if (idx < 0 || idx >= kNumKeys || chan < 0 || chan >= 16) return;

    if (!m_pressedKeys[chan][idx]) {
        m_audioOutput->noteOn(chan, midi_note_number, velocity);
        m_pressedKeys[chan][idx] = true;
        emit noteChannelsChanged(midi_note_number, getNotePressingChannels(midi_note_number));
        emit pressedNotesChanged(getPressedNotes());
    }
}

void Piano::keyOffInternal(int midi_note_number, int chan)
{
    int idx = noteToIndex(midi_note_number);
    if (idx < 0 || idx >= kNumKeys || chan < 0 || chan >= 16) return;

    if (m_pressedKeys[chan][idx]) {
        m_audioOutput->noteOff(chan, midi_note_number);
        m_pressedKeys[chan][idx] = false;
        emit noteChannelsChanged(midi_note_number, getNotePressingChannels(midi_note_number));
        emit pressedNotesChanged(getPressedNotes());
    }
}

void Piano::programChange(int chan, int program)
{
    m_audioOutput->programChange(chan, program);
}
