// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QStringList>
#include <vector>
#include "utils.hpp"

class ChordIdentifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList chordNames READ chordNames NOTIFY chordNamesChanged)

public:
    explicit ChordIdentifier(QObject* parent = nullptr);

    QStringList chordNames() const;

public slots:
    void updatePressedNotes(const std::vector<size_t>& pressedNotes);

signals:
    void chordNamesChanged();

private:
    QStringList m_chordNames;
    Key m_key = Key::CMajor; // Default key, can be made configurable later
};
