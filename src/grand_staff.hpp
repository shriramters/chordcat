// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QQuickPaintedItem>
#include <QFont>
#include <QPixmap>
#include <vector>
#include "key.hpp"

class GrandStaff : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit GrandStaff(QQuickItem* parent = nullptr);

public slots:
    Q_INVOKABLE void setKey(int keyIndex);
    void updateNotes(const std::vector<size_t>& pressedMidiNotes);
    void setNotes(const std::vector<int>& midiNotes);
    void updateStaffColor();

protected:
    // QQuickPaintedItem interface:
    void paint(QPainter* painter) override;
    void componentComplete() override;

private:
    void drawStaff(QPainter& painter) const;
    void drawKeySignature(QPainter& painter) const;
    void drawNotes(QPainter& painter) const;

    float noteYPosition(int midiNote) const;
    bool isNatural(int midiNote) const;
    QString getAccidentalGlyph(int midiNote) const;
    int stepsFromRef(int midiNote, int ref) const;
    void midiToLetterOctave(int midiNote, int& letter, int& octave) const;

private:
    Key currentKey = Key::CMajor;
    std::vector<int> displayedNotes = {};
    float staffLeftX     = 0.f;
    float staffTopY      = 0.f;
    float staffSpacing   = 0.f;
    float noteRadius     = 0.f;
    float gapBetweenStaves = 0.f;

    QFont font;
    QColor staffColor = Qt::blue;
    QPixmap trebleClef;
    QPixmap bassClef;
};
