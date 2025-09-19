import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Chordcat 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 600 * 16 / 9
    height: 600
    title: "ChordQt"

    Component.onCompleted: {
        // Set initial values from settings
        pianoBackend.setGain(AppSettings.synth_gain)

        const sfPath = soundFontManager.getSoundFontPath(AppSettings.soundfont_name)
        if (sfPath) {
            pianoBackend.loadSoundFont(sfPath)
        } else if (soundFontManager.availableSoundFonts.length > 0) {
            // If saved soundfont not found, use the first available one
            const firstSfName = soundFontManager.availableSoundFonts[0]
            const firstSfPath = soundFontManager.getSoundFontPath(firstSfName)
            AppSettings.soundfont_name = firstSfName
            pianoBackend.loadSoundFont(firstSfPath)
        }

        grandStaff.setKey(AppSettings.keySignature)
        midiManager.currentBackendIndex = AppSettings.midi_backendIndex
    }

    Connections {
        target: AppSettings
        function onSynth_gainChanged() { pianoBackend.setGain(AppSettings.synth_gain); }
        function onSoundfont_nameChanged() {
            const path = soundFontManager.getSoundFontPath(AppSettings.soundfont_name);
            if (path) {
                pianoBackend.loadSoundFont(path);
            }
        }
        function onKeySignatureChanged() { grandStaff.setKey(AppSettings.keySignature); }
    }

    Connections {
        target: midiManager

        function onCurrentBackendChanged() {
            AppSettings.midi_backendIndex = midiManager.currentBackendIndex;
        }
        function onPortChanged() {
            AppSettings.midi_portIndex = midiManager.currentPortIndex;
        }

        function onPortsChanged() {
            if (midiManager.availablePorts.length > 0) {
                if (AppSettings.midi_portIndex >= 0 && AppSettings.midi_portIndex < midiManager.availablePorts.length) {
                    midiManager.currentPortIndex = AppSettings.midi_portIndex;
                } else {
                    midiManager.currentPortIndex = 0;
                }
            } else {
                midiManager.currentPortIndex = -1;
            }
        }
    }

    property bool showingPreferences: false
    onShowingPreferencesChanged: {
        // always keep focus with piano
        pianoKeyboard.forceActiveFocus()
    }

    FontLoader {
        id: koulen
        source: "qrc:/assets/fonts/Koulen/Koulen-Regular.ttf"
    }

    RowLayout {
        id: topRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 32
        anchors.rightMargin: 32
        anchors.topMargin: 8
        anchors.bottomMargin: 8
        spacing: 0

        Item {
            id: bannerContainer
            implicitHeight: chordCatText.implicitHeight
            implicitWidth: Math.max(chordCatText.implicitWidth, prefsText.implicitWidth)
            Layout.preferredWidth: implicitWidth
            clip: true

            state: showingPreferences ? "PreferencesState" : "ChordCatState"

            Text {
                id: chordCatText
                text: "CHORDCAT"
                color: window.palette.text
                font.pixelSize: 48
                font.family: koulen.name
            }

            Text {
                id: prefsText
                text: "PREFERENCES"
                color: window.palette.text
                font.pixelSize: 48
                font.family: koulen.name
            }

            states: [
                State {
                    name: "ChordCatState"
                    PropertyChanges { target: chordCatText; y: 0; opacity: 1.0 }
                    PropertyChanges { target: prefsText; y: bannerContainer.height; opacity: 0.0 }
                },
                State {
                    name: "PreferencesState"
                    PropertyChanges { target: chordCatText; y: -bannerContainer.height; opacity: 0.0 }
                    PropertyChanges { target: prefsText; y: 0; opacity: 1.0 }
                }
            ]

            transitions: Transition {
                // These animations will run in parallel for any state change
                ParallelAnimation {
                    NumberAnimation {
                        properties: "y, opacity"
                        duration: 250
                        easing.type: Easing.InOutCubic
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }
        RoundButton {
            id: prefsButton
            icon.source: showingPreferences ? "qrc:/assets/images/check.png" : "qrc:/assets/images/gear.png"
            onClicked: {
                showingPreferences = !showingPreferences
            }
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            icon.width: 32
            icon.height: 32
        }
    }

    // Content area anchored between topRow and pianoKeyboard
    StackLayout {
        id: contentStack
        anchors {
            top: topRow.bottom
            bottom: pianoKeyboard.top
            left: parent.left
            right: parent.right
            margins: 32
        }
        currentIndex: showingPreferences ? 1 : 0

        RowLayout {
            id: contentRow
            spacing: 48

            ListView {
                id: chordNamesView
                Layout.preferredWidth: window.width * 0.5
                Layout.fillHeight: true
                model: chordIdentifier.chordNames
                delegate: Text {
                    text: modelData
                    font.pixelSize: 48
                    color: window.palette.text
                }
                ScrollIndicator.vertical: ScrollIndicator { }
            }

            GrandStaff {
                id: grandStaff
                Layout.fillWidth: true
                Layout.fillHeight: true
                Connections {
                    target: pianoBackend
                    function onPressedNotesChanged(pressedNotes) {
                        grandStaff.updateNotes(pressedNotes)
                    }
                }
            }
        }
        Preferences {
            id: preferencesPage
        }
    }


    PianoKeyboard {
        id: pianoKeyboard
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
