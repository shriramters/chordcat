import QtQuick
import QtQuick.Controls


Item {
    id: root
    focus: true
    height: whiteKeyHeight
    width: 1000

    property real whiteKeyWidth: width / 52
    property real blackKeyWidth: whiteKeyWidth * 0.5
    property real keyAspectRatio: 4.0
    property real whiteKeyHeight: whiteKeyWidth * keyAspectRatio
    property real blackKeyHeight: blackKeyWidth * keyAspectRatio

    function isBlackKey(noteIndex) {
        switch (noteIndex % 12) {
        case 1:
        case 4:
        case 6:
        case 9:
        case 11:
            return true
        default:
            return false
        }
    }

    

    // White keys
    Repeater {
        id: whiteKeys
        model: 88

        delegate: Rectangle {
            visible: !root.isBlackKey(model.index)
            property int midiNote: model.index + 21
            property bool pressed: pianoBackend.isNotePressed(midiNote)

            color: pressed ? AppSettings.piano_pressedNoteColor : "white"
            border.color: "#555"
            border.width: 1

            // Lay out each white key side by side
            x: {
                var count = 0
                for (var i = 0; i < model.index; i++) {
                    if (!root.isBlackKey(i)) {
                        count++
                    }
                }
                return count * root.whiteKeyWidth
            }
            y: 0
            width: root.whiteKeyWidth
            height: parent.height

            // Text {
            //     text: midiNote
            //     anchors.bottom: parent.bottom
            //     anchors.horizontalCenter: parent.horizontalCenter
            //     color: pressed ? "red" : "black"
            //     font.pointSize: 8
            // }

            MouseArea {
                anchors.fill: parent
                onPressed: pianoBackend.keyToggle(midiNote)
            }

            Connections {
                target: pianoBackend
                function onNoteStateChanged(note, isOn) {
                    if (midiNote === note) {
                        pressed = isOn
                    }
                }
            }
        }
    }

    // Black keys
    Repeater {
        id: blackKeys
        model: 88

        delegate: Item {
            visible: root.isBlackKey(model.index)
            z: 999
            property int midiNote: model.index + 21
            property bool pressed: pianoBackend.isNotePressed(midiNote)

            width: root.blackKeyWidth
            height: root.blackKeyHeight

            x: {
                var wCount = 0
                for (var j = 0; j < model.index; j++) {
                    if (!root.isBlackKey(j))
                        wCount++
                }
                return (wCount - 1) * root.whiteKeyWidth
                        + (root.whiteKeyWidth - root.blackKeyWidth * 0.5)
            }
            y: 0

            Rectangle {
                anchors.fill: parent
                x: 2
                y: 2
                color: "#00000044"
            }

            Rectangle {
                id: blackKey
                anchors.fill: parent
                color: pressed ? "#ed4545" : "#333"
                border.color: "#555"
                border.width: 1

                // Text {
                //     text: midiNote
                //     anchors.bottom: parent.bottom
                //     anchors.horizontalCenter: parent.horizontalCenter
                //     color: pressed ? "yellow" : "white"
                //     font.pointSize: 8
                // }
                MouseArea {
                    anchors.fill: parent
                    onPressed: pianoBackend.keyToggle(midiNote)
                }

                Connections {
                    target: pianoBackend
                    function onNoteStateChanged(note, isOn) {
                        if (midiNote === note) {
                            pressed = isOn
                        }
                    }
                }
            }
        }
    }

    property int octave: 3

    function getNoteFromKeyCode(key) {
        switch (key) {
        case Qt.Key_Z:
            return 0
        case Qt.Key_S:
            return 1
        case Qt.Key_X:
            return 2
        case Qt.Key_D:
            return 3
        case Qt.Key_C:
            return 4
        case Qt.Key_V:
            return 5
        case Qt.Key_G:
            return 6
        case Qt.Key_B:
            return 7
        case Qt.Key_H:
            return 8
        case Qt.Key_N:
            return 9
        case Qt.Key_J:
            return 10
        case Qt.Key_M:
            return 11
        case Qt.Key_Q:
            return 12
        case Qt.Key_2:
            return 13
        case Qt.Key_W:
            return 14
        case Qt.Key_3:
            return 15
        case Qt.Key_E:
            return 16
        case Qt.Key_R:
            return 17
        case Qt.Key_5:
            return 18
        case Qt.Key_T:
            return 19
        case Qt.Key_6:
            return 20
        case Qt.Key_Y:
            return 21
        case Qt.Key_7:
            return 22
        case Qt.Key_U:
            return 23
        case Qt.Key_I:
            return 24
        case Qt.Key_9:
            return 25
        case Qt.Key_O:
            return 26
        case Qt.Key_0:
            return 27
        case Qt.Key_P:
            return 28
        default:
            return -1
        }
    }

    Keys.onPressed: function (event) {
        if (event.isAutoRepeat)
            return
        if (event.key === Qt.Key_Minus) {
            octave = Math.max(octave - 1, 0)
        } else if (event.key === Qt.Key_Equal) {
            octave = Math.min(octave + 1, 6)
        } else {
            var noteOffset = getNoteFromKeyCode(event.key)
            if (noteOffset >= 0) {
                pianoBackend.keyOn(noteOffset + octave * 12, AppSettings.pc_keyboard_channel, 100)
            }
        }
        event.accepted = true
    }

    Keys.onReleased: function (event) {
        if (!event.isAutoRepeat) {
            var noteOffset = getNoteFromKeyCode(event.key)
            if (noteOffset >= 0) {
                pianoBackend.keyOff(noteOffset + octave * 12, AppSettings.pc_keyboard_channel)
            }
        }
        event.accepted = true
    }
}
