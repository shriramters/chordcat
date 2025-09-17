// SPDX-License-Identifier: GPL-3.0-only
pragma Singleton

import QtCore
import QtQuick

Settings {
    // Piano settings
    property real piano_gain: 1.0
    property color piano_pressedNoteColor: "#ed4545"
    property int piano_channel: 0

    // UI settings
    property string fontName: "Koulen"
    property string fontPath: "qrc:/assets/fonts/Koulen/Koulen-Regular.ttf"

    // MIDI settings
    property int midi_backendIndex: 0
    property int midi_portIndex: -1

    // SoundFont settings
    property string soundfont_name: "TimGM6mb.sf2"

    // Other settings
    property int keySignature: 0 // C Major
    property int metronomeBpm: 120
}
