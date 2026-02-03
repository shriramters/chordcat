// SPDX-License-Identifier: GPL-3.0-only
#include "grand_staff.hpp"
// #include "config.h"
#include <QPainter>
#include <QUrl>
#include <QDebug>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlProperty>
#include <QColor>
#include <cmath>
#include <map>

std::map<Key, int> gKeySharpsFlats = {
    {Key::CMajor, 0},      {Key::GMajor, 1},       {Key::DMajor, 2},      {Key::AMajor, 3},
    {Key::EMajor, 4},      {Key::BMajor, 5},       {Key::FSharpMajor, 6}, {Key::CSharpMajor, 7},
    {Key::FMajor, -1},     {Key::BbMajor, -2},     {Key::EbMajor, -3},    {Key::AbMajor, -4},
    {Key::DbMajor, -5},    {Key::GbMajor, -6},     {Key::CbMajor, -7},    {Key::DSharpMajor, 6},
    {Key::GSharpMajor, 8}, {Key::ASharpMajor, 10},
    };


GrandStaff::GrandStaff(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    trebleClef.load(":/assets/images/treble_clef.png");
    bassClef.load(":/assets/images/bass_clef.png");
    setAntialiasing(true); // For smoother lines
}

void GrandStaff::componentComplete()
{
    QQuickPaintedItem::componentComplete();

    QQuickWindow* win = window();

    if (win) {
        // listen for palette changes
        connect(win, &QQuickWindow::paletteChanged,
                this, &GrandStaff::updateStaffColor);
        // update initial staff pen color
        updateStaffColor();
    }
}

void GrandStaff::updateStaffColor()
{
    QQuickWindow* win = window();
    if (!win) return; // Safety check

    staffColor = QQmlProperty::read(win, "palette.text").value<QColor>();

    // repaint
    update();
}

void GrandStaff::setKey(int keyIndex)
{
    Key key = static_cast<Key>(keyIndex);
    if (currentKey != key) {
        currentKey = key;
        update(); // triggers repaint
    }
}

void GrandStaff::updateNotes(const std::vector<size_t>& pressedMidiNotes)
{
    // We'll compute geometry in paint(). For now, just store notes:
    std::vector<int> notes;
    notes.reserve(pressedMidiNotes.size());
    for (auto n : pressedMidiNotes) {
        notes.push_back(int(n + 21)); // like your SFML code: index + 21
    }
    displayedNotes = notes;
    update();
}

void GrandStaff::setNotes(const std::vector<int>& midiNotes)
{
    displayedNotes = midiNotes;
    update();
}

/**
 * The QQuickPaintedItem "draw" method.
 * Replaces the SFML virtual "draw(sf::RenderTarget&, sf::RenderStates)".
 */
void GrandStaff::paint(QPainter *painter)
{
    // Dynamically compute geometry based on the current item dimensions.
    float horizontalPadding = width() * 0.25f;
    staffLeftX      = horizontalPadding;
    staffTopY       = height() * 0.1f;
    noteRadius      = width() * 0.01f;
    staffSpacing    = height() * 0.05f;
    noteRadius      = staffSpacing / 2.0f;
    gapBetweenStaves = 2 * staffSpacing;

    drawStaff(*painter);
    drawKeySignature(*painter);
    drawNotes(*painter);
}


void GrandStaff::drawStaff(QPainter& painter) const
{
    painter.setRenderHint(QPainter::Antialiasing, false);

    //WAS: lineLength = target.getView().getSize().x / 10.f;
    //Then WAS: lineLength = width()/2.5f;
    float lineLength = width() - (2 * staffLeftX);

    float lineThickness = staffSpacing * 0.05f;
    QPen linePen(staffColor, lineThickness);
    painter.setPen(linePen);

    // Treble staff lines
    for (int i = 0; i < 5; i++) {
        float y = staffTopY + i * staffSpacing;
        painter.drawLine(QPointF(staffLeftX, y),
                         QPointF(staffLeftX + lineLength, y));
    }

    // Bass staff lines
    float bassTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;
    for (int i = 0; i < 5; i++) {
        float y = bassTop + i * staffSpacing;
        painter.drawLine(QPointF(staffLeftX, y),
                         QPointF(staffLeftX + lineLength, y));
    }

    // The bracket on the left
    float bracketWidth = 5.f;
    float bracketHeight = (bassTop + (4 * staffSpacing) - staffTopY) + lineThickness;
    painter.fillRect(QRectF(staffLeftX - bracketWidth,
                            staffTopY - lineThickness * 0.5,
                            bracketWidth,
                            bracketHeight),
                     staffColor);

    // If trebleClef/bassClef QPixmaps loaded, draw them:
    if (!trebleClef.isNull()) {
        float scaleFactor = (5 * staffSpacing) / trebleClef.height();
        float w = trebleClef.width() * scaleFactor;
        float h = trebleClef.height() * scaleFactor;
        float x = staffLeftX - 10.f - w;
        float y = staffTopY;
        painter.drawPixmap(QRectF(x, y, w, h), trebleClef, trebleClef.rect());
    }
    if (!bassClef.isNull()) {
        float bassY = bassTop;
        float scaleFactor = (5 * staffSpacing) / bassClef.height();
        float w = bassClef.width() * scaleFactor;
        float h = bassClef.height() * scaleFactor;
        float x = staffLeftX - 10.f - w;
        painter.drawPixmap(QRectF(x, bassY, w, h), bassClef, bassClef.rect());
    }
}

void GrandStaff::drawKeySignature(QPainter& painter) const
{
    int count = gKeySharpsFlats.at(currentKey);
    if (count == 0)
        return;

    float bassTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;

    QFont textFont;
    textFont.setPixelSize(int(staffSpacing * 1.5f));
    painter.setFont(textFont);
    painter.setPen(staffColor);

    static float trebleSharpY[7] = {1.f, 4.f, 0.f, 3.f, 6.f, 2.f, 5.f};
    static float trebleFlatY[7]  = {5.f, 2.f, 6.f, 3.f, 7.f, 4.f, 8.f};

    float trebleBase = staffTopY - (staffSpacing * 0.25f);
    float bassBase   = bassTop - (staffSpacing * 0.25f);

    int howMany = std::abs(count);
    if (count > 0) {
        // Sharps
        QString sharpSym = QString::fromWCharArray(L"♯");
        for (int i = 0; i < howMany && i < 7; i++) {
            float tx = staffLeftX + i * (staffSpacing * 0.75f);
            float tyTreble = trebleBase + (trebleSharpY[i] * 0.5f * staffSpacing);
            painter.drawText(QPointF(tx, tyTreble), sharpSym);

            float tyBass = bassBase + ((trebleSharpY[i] + 2) * 0.5f * staffSpacing);
            painter.drawText(QPointF(tx, tyBass), sharpSym);
        }
    } else {
        // Flats
        QString flatSym = QString::fromWCharArray(L"♭");
        for (int i = 0; i < howMany && i < 7; i++) {
            float tx = staffLeftX + i * (staffSpacing * 0.75f);
            float tyTreble = trebleBase + (trebleFlatY[i] * 0.5f * staffSpacing);
            painter.drawText(QPointF(tx, tyTreble), flatSym);

            float tyBass = bassBase + (trebleFlatY[i] * 0.5f * staffSpacing);
            painter.drawText(QPointF(tx, tyBass), flatSym);
        }
    }
}

void GrandStaff::drawNotes(QPainter& painter) const
{
    float lineLength = width() - (2 * staffLeftX);
    float noteX = staffLeftX + (lineLength / 2.0f);

    QBrush noteBrush(staffColor);
    painter.setBrush(noteBrush);
    painter.setPen(Qt::NoPen);

    // For accidental text
    QFont accidentalFont;
    accidentalFont.setPixelSize(int(staffSpacing * 1.5f));
    painter.setFont(accidentalFont);
    painter.setPen(staffColor);
    painter.setRenderHint(QPainter::Antialiasing, true);

    for (auto midiNote : displayedNotes) {
        float y = noteYPosition(midiNote);

        // Draw oval note
        float rX = noteRadius;
        float rY = noteRadius * 0.75f;
        QRectF noteRect(noteX - rX, y - rY, rX*2, rY*2);
        painter.drawEllipse(noteRect);

        // Draw accidental if needed (shifted down by one line)
        QString acc = getAccidentalGlyph(midiNote);
        if (!acc.isEmpty()) {
            float ax = noteX - (staffSpacing * 1.5f);
            float ay = y + (staffSpacing * 0.25f);
            painter.drawText(QPointF(ax, ay), acc);
        }
    }
}

float GrandStaff::noteYPosition(int midiNote) const
{
    bool isBassClef = (midiNote < 60);
    int referenceNote = isBassClef ? 43 : 64;
    int stepCount = stepsFromRef(midiNote, referenceNote);

    float bassStaffTop = staffTopY + 5 * staffSpacing + gapBetweenStaves;
    float trebleStaffBottom = staffTopY + 4 * staffSpacing;
    float bassStaffBottom   = bassStaffTop + 4 * staffSpacing;
    float verticalOffset    = stepCount * (staffSpacing / 2.f);

    return isBassClef
               ? (bassStaffBottom - verticalOffset)
               : (trebleStaffBottom - verticalOffset);
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

QString GrandStaff::getAccidentalGlyph(int midiNote) const {
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
        return QString();

    if (isNatural(midiNote))
        return "♮";
    else {
        if (is_sharp_key(currentKey))
            return "♯";
        else
            return "♭";
    }
}

int GrandStaff::stepsFromRef(int midiNote, int ref) const
{
    int l1, o1, l2, o2;
    midiToLetterOctave(midiNote, l1, o1);
    midiToLetterOctave(ref, l2, o2);
    return (l1 + 7 * o1) - (l2 + 7 * o2);
}

void GrandStaff::midiToLetterOctave(int midiNote, int& letter, int& octave) const
{
    int h = (midiNote - 60) % 12;
    if (h < 0)
        h += 12;

    static int mapSharps[12] = {0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6};
    static int mapFlats[12]  = {0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6};

    if (is_sharp_key(currentKey))
        letter = mapSharps[h];
    else
        letter = mapFlats[h];

    octave = (midiNote / 12) - 1;
}
