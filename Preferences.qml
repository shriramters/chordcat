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
            Layout.preferredWidth: 250
            Layout.fillHeight: true

            ListView {
                id: navigationList
                anchors.fill: parent
                clip: true
                model: ["Input", "Audio", "UI"]
                currentIndex: 0

                delegate: ItemDelegate {
                    width: parent.width
                    text: modelData
                    font.pixelSize: 28
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
            
            // Input Settings
            Frame {
                padding: 12
                ColumnLayout {
                    Label { text: "MIDI Input"; font.pixelSize: 20; topPadding:0; bottomPadding: 12 }
                    GridLayout {
                        columns: 2
                        Label { text: "Backend"; font.bold: true }
                        ComboBox {
                            focusPolicy: Qt.NoFocus
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
                            focusPolicy: Qt.NoFocus
                            model: midiManager.availablePorts
                            currentIndex: midiManager.currentPortIndex
                            Layout.fillWidth: true
                        }
                    }

                    MenuSeparator {
                        padding: 0
                        topPadding: 12
                        bottomPadding: 12
                        contentItem: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 1
                            opacity: 0.5
                        }
                    }

                    Label { text: "PC Keyboard Input"; font.pixelSize: 20; topPadding:0; bottomPadding: 12 }
                    GridLayout {
                        columns: 2
                        Label { text: "Channel" }
                        SpinBox {
                            focusPolicy: Qt.NoFocus
                            from: 0
                            to: 15
                            value: AppSettings.pc_keyboard_channel
                            onValueChanged: {
                                AppSettings.pc_keyboard_channel = value;
                                pianoBackend.programChange(value, AppSettings.pc_keyboard_program);
                            }
                        }

                        Label { text: "Instrument" }
                        RowLayout {
                            SpinBox {
                                focusPolicy: Qt.NoFocus
                                from: 0
                                to: 127
                                value: AppSettings.pc_keyboard_program
                                onValueChanged: {
                                    AppSettings.pc_keyboard_program = value;
                                    pianoBackend.programChange(AppSettings.pc_keyboard_channel, value);
                                }
                            }
                            RoundButton {
                                icon.source: "qrc:/assets/images/info.png"
                                icon.width: 16
                                icon.height: 16
                                onClicked: helpWindow.open()
                            }
                        }
                    }
                }
            }

            // Audio Settings
            Frame {
                padding: 12
                ColumnLayout {
                    Label { text: "FluidSynth Settings"; font.pixelSize: 20; topPadding:0; bottomPadding: 12 }
                    GridLayout {
                        columns: 2
                        Label { text: "Synth Gain"; font.bold: true }
                        Slider {
                            focusPolicy: Qt.NoFocus
                            from: 0.0
                            to: 10.0
                            value: AppSettings.synth_gain
                            onValueChanged: AppSettings.synth_gain = value
                            Layout.fillWidth: true
                        }

                        Label { text: "SoundFont"; font.bold: true }
                        ComboBox {
                            focusPolicy: Qt.NoFocus
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
            }

            // User Interface Settings
            Frame {
                padding: 12
                ColumnLayout {
                    Label { text: "Notation"; font.pixelSize: 20; topPadding:0; bottomPadding: 12 }
                    GridLayout {
                        columns: 2

                        Label { text: "Key Signature"; font.bold: true }
                        ComboBox {
                            focusPolicy: Qt.NoFocus
                            model: keySignatureModel
                            currentIndex: AppSettings.keySignature
                            onCurrentIndexChanged: AppSettings.keySignature = currentIndex
                            Layout.fillWidth: true
                        }
                    }

                    MenuSeparator {
                        padding: 0
                        topPadding: 12
                        bottomPadding: 12
                        contentItem: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 1
                            opacity: 0.5
                        }
                    }

                    Label { text: "Keyboard UI"; font.pixelSize: 20; topPadding:0; bottomPadding: 12 }
                    GridLayout {
                        columns: 2
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
    }

    Popup {
        id: helpWindow
        width: 640
        height: 480
        modal: true
        anchors.centerIn: parent
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        ColumnLayout {
            anchors.fill: parent
            Label {
                text: "GM Instrument Table"
                font.pixelSize: 20
                padding: 12
                Layout.alignment: Qt.AlignHCenter
            }
            InstrumentTable {
                Layout.fillWidth: true
                Layout.fillHeight: true
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
