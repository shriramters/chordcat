// SPDX-License-Identifier: GPL-3.0-only
#include "chord_identifier.hpp"

ChordIdentifier::ChordIdentifier(QObject* parent)
    : QObject(parent)
{
}

QStringList ChordIdentifier::chordNames() const
{
    return m_chordNames;
}

void ChordIdentifier::updatePressedNotes(const std::vector<size_t>& pressedNotes)
{
    m_chordNames.clear();
    auto chordset = name_that_chord(pressedNotes);
    for (const auto& chord : chordset) {
        m_chordNames.append(chord.toString(m_key));
    }
    emit chordNamesChanged();
}
