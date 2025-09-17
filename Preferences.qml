// SPDX-License-Identifier: GPL-3.0-only
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Chordcat 1.0

Pane {
    id: rootPane
    implicitWidth: 600
    implicitHeight: 400

    property var keySignatureModel: [
        "C Major / A Minor",   "G Major / E Minor",   "D Major / B Minor",
        "A Major / F# Minor",  "E Major / C# Minor",  "B Major / G# Minor",
        "F# Major / D# Minor", "C# Major / A# Minor", "F Major / D Minor",
        "Bb Major / G Minor",  "Eb Major / C Minor",  "Ab Major / F Minor",
        "Db Major / Bb Minor", "Gb Major / Eb Minor", "Cb Major / Ab Minor",
        "D# Major / B# Minor", "G# Major / E# Minor", "A# Major / Fx Minor"
    ]

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Pane {
            Layout.preferredWidth: 200
            Layout.fillHeight: true

            ListView {
                id: navigationList
                anchors.fill: parent
                clip: true
                model: ["MIDI", "Audio", "Interface"]
                currentIndex: 0

                delegate: ItemDelegate {
                    width: parent.width
                    text: modelData
                    font.pixelSize: 24
                    topPadding: 12
                    bottomPadding: 12
                    highlighted: ListView.isCurrentItem
                    onClicked: navigationList.currentIndex = index
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: navigationList.currentIndex
            
            // MIDI Settings
            Frame {
                padding: 20
                GridLayout {
                    columns: 2
                    
                    Label { text: "Backend"; font.bold: true }
                    ComboBox {
                        model: midiManager.availableBackends
                        currentIndex: midiManager.currentBackendIndex

                        onCurrentIndexChanged: {
                            if (currentIndex !== midiManager.currentBackendIndex) {
                                // Port MUST be reset before backend is changed.
                                midiManager.currentPortIndex = -1;
                                // then, change the backend.
                                midiManager.currentBackendIndex = currentIndex;
                            }
                        }
                        Layout.fillWidth: true
                    }

                    Label { text: "Input Port"; font.bold: true }
                    ComboBox {
                        model: midiManager.availablePorts
                        currentIndex: midiManager.currentPortIndex
                        Layout.fillWidth: true
                    }
                }
            }

            // Audio Settings
            Frame {
                padding: 20
                GridLayout {
                    columns: 2

                    Label { text: "Piano Gain"; font.bold: true }
                    Slider {
                        from: 0.0
                        to: 10.0
                        value: AppSettings.piano_gain
                        onValueChanged: AppSettings.piano_gain = value
                        Layout.fillWidth: true
                    }

                    Label { text: "SoundFont"; font.bold: true }
                    ComboBox {
                        model: soundFontManager.availableSoundFonts
                        currentIndex: model.length > 0 ? (model.indexOf(AppSettings.soundfont_name) > -1 ? model.indexOf(AppSettings.soundfont_name) : 0) : -1

                        onCurrentIndexChanged: {
                            if (currentIndex > -1) {
                                AppSettings.soundfont_name = model[currentIndex]
                            }
                        }
                        Layout.fillWidth: true
                    }
                }
            }

            // Interface Settings
            Frame {
                padding: 20
                GridLayout {
                    columns: 2

                    Label { text: "Key Signature"; font.bold: true }
                    ComboBox {
                        model: keySignatureModel
                        currentIndex: AppSettings.keySignature
                        onCurrentIndexChanged: AppSettings.keySignature = currentIndex
                        Layout.fillWidth: true
                    }

                    Label { text: "Pressed Note Color"; font.bold: true }
                    Rectangle {
                        width: 120
                        height: 32
                        color: AppSettings.piano_pressedNoteColor
                        border.color: "gray"
                        radius: 4
                        MouseArea {
                            anchors.fill: parent
                            onClicked: colorDialog.open()
                        }
                    }
                }
            }
        }
    }

    ColorDialog {
        id: colorDialog
        title: "Select a color"
        selectedColor: AppSettings.piano_pressedNoteColor
        onAccepted: {
            AppSettings.piano_pressedNoteColor = selectedColor
        }
    }
}
