import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Pane {
  id: rootPane
  implicitWidth: 600
  implicitHeight: 400
  property var instrumentData: {
    "categories": [
      {
        "name": "Piano",
        "instruments": [
          { "code": 0, "name": "Acoustic Grand Piano" },
          { "code": 1, "name": "Bright Acoustic Piano" },
          { "code": 2, "name": "Electric Grand Piano" },
          { "code": 3, "name": "Honky-tonk Piano" },
          { "code": 4, "name": "Electric Piano 1" },
          { "code": 5, "name": "Electric Piano 2" },
          { "code": 6, "name": "Harpsichord" },
          { "code": 7, "name": "Clavinet" }
        ]
      },
      {
        "name": "Chromatic Percussion",
        "instruments": [
          { "code": 8, "name": "Celesta" },
          { "code": 9, "name": "Glockenspiel" },
          { "code": 10, "name": "Music Box" },
          { "code": 11, "name": "Vibraphone" },
          { "code": 12, "name": "Marimba" },
          { "code": 13, "name": "Xylophone" },
          { "code": 14, "name": "Tubular Bells" },
          { "code": 15, "name": "Dulcimer" }
        ]
      },
      {
        "name": "Organ",
        "instruments": [
          { "code": 16, "name": "Drawbar Organ" },
          { "code": 17, "name": "Percussive Organ" },
          { "code": 18, "name": "Rock Organ" },
          { "code": 19, "name": "Church Organ" },
          { "code": 20, "name": "Reed Organ" },
          { "code": 21, "name": "Accordion" },
          { "code": 22, "name": "Harmonica" },
          { "code": 23, "name": "Bandoneon" }
        ]
      },
      {
        "name": "Guitar",
        "instruments": [
          { "code": 24, "name": "Acoustic Guitar (nylon)" },
          { "code": 25, "name": "Acoustic Guitar (steel)" },
          { "code": 26, "name": "Electric Guitar (jazz)" },
          { "code": 27, "name": "Electric Guitar (clean)" },
          { "code": 28, "name": "Electric Guitar (muted)" },
          { "code": 29, "name": "Electric Guitar (overdrive)" },
          { "code": 30, "name": "Electric Guitar (distortion)" },
          { "code": 31, "name": "Electric Guitar (harmonics)" }
        ]
      },
      {
        "name": "Bass",
        "instruments": [
          { "code": 32, "name": "Acoustic Bass" },
          { "code": 33, "name": "Electric Bass (finger)" },
          { "code": 34, "name": "Electric Bass (picked)" },
          { "code": 35, "name": "Electric Bass (fretless)" },
          { "code": 36, "name": "Slap Bass 1" },
          { "code": 37, "name": "Slap Bass 2" },
          { "code": 38, "name": "Synth Bass 1" },
          { "code": 39, "name": "Synth Bass 2" }
        ]
      },
      {
        "name": "Strings",
        "instruments": [
          { "code": 40, "name": "Violin" },
          { "code": 41, "name": "Viola" },
          { "code": 42, "name": "Cello" },
          { "code": 43, "name": "Contrabass" },
          { "code": 44, "name": "Tremolo Strings" },
          { "code": 45, "name": "Pizzicato Strings" },
          { "code": 46, "name": "Orchestral Harp" },
          { "code": 47, "name": "Timpani" }
        ]
      },
      {
        "name": "Ensemble",
        "instruments": [
          { "code": 48, "name": "String Ensemble 1" },
          { "code": 49, "name": "String Ensemble 2" },
          { "code": 50, "name": "Synth Strings 1" },
          { "code": 51, "name": "Synth Strings 2" },
          { "code": 52, "name": "Choir Aahs" },
          { "code": 53, "name": "Voice Oohs" },
          { "code": 54, "name": "Synth Voice" },
          { "code": 55, "name": "Orchestra Hit" }
        ]
      },
      {
        "name": "Brass",
        "instruments": [
          { "code": 56, "name": "Trumpet" },
          { "code": 57, "name": "Trombone" },
          { "code": 58, "name": "Tuba" },
          { "code": 59, "name": "Muted Trumpet" },
          { "code": 60, "name": "French Horn" },
          { "code": 61, "name": "Brass Section" },
          { "code": 62, "name": "Synth Brass 1" },
          { "code": 63, "name": "Synth Brass 2" }
        ]
      },
      {
        "name": "Reed",
        "instruments": [
          { "code": 64, "name": "Soprano Sax" },
          { "code": 65, "name": "Alto Sax" },
          { "code": 66, "name": "Tenor Sax" },
          { "code": 67, "name": "Baritone Sax" },
          { "code": 68, "name": "Oboe" },
          { "code": 69, "name": "English Horn" },
          { "code": 70, "name": "Bassoon" },
          { "code": 71, "name": "Clarinet" }
        ]
      },
      {
        "name": "Pipe",
        "instruments": [
          { "code": 72, "name": "Piccolo" },
          { "code": 73, "name": "Flute" },
          { "code": 74, "name": "Recorder" },
          { "code": 75, "name": "Pan Flute" },
          { "code": 76, "name": "Blown Bottle" },
          { "code": 77, "name": "Shakuhachi" },
          { "code": 78, "name": "Whistle" },
          { "code": 79, "name": "Ocarina" }
        ]
      },
      {
        "name": "Synth Lead",
        "instruments": [
          { "code": 80, "name": "Lead 1 (square)" },
          { "code": 81, "name": "Lead 2 (sawtooth)" },
          { "code": 82, "name": "Lead 3 (calliope)" },
          { "code": 83, "name": "Lead 4 (chiff)" },
          { "code": 84, "name": "Lead 5 (charang)" },
          { "code": 85, "name": "Lead 6 (voice)" },
          { "code": 86, "name": "Lead 7 (fifths)" },
          { "code": 87, "name": "Lead 8 (bass and lead)" }
        ]
      },
      {
        "name": "Synth Pad",
        "instruments": [
          { "code": 88, "name": "Pad 1 (new age)" },
          { "code": 89, "name": "Pad 2 (warm)" },
          { "code": 90, "name": "Pad 3 (polysynth)" },
          { "code": 91, "name": "Pad 4 (choir)" },
          { "code": 92, "name": "Pad 5 (bowed glass)" },
          { "code": 93, "name": "Pad 6 (metallic)" },
          { "code": 94, "name": "Pad 7 (halo)" },
          { "code": 95, "name": "Pad 8 (sweep)" }
        ]
      },
      {
        "name": "Synth Effects",
        "instruments": [
          { "code": 96, "name": "FX 1 (rain)" },
          { "code": 97, "name": "FX 2 (soundtrack)" },
          { "code": 98, "name": "FX 3 (crystal)" },
          { "code": 99, "name": "FX 4 (atmosphere)" },
          { "code": 100, "name": "FX 5 (brightness)" },
          { "code": 101, "name": "FX 6 (goblins)" },
          { "code": 102, "name": "FX 7 (echoes)" },
          { "code": 103, "name": "FX 8 (sci-fi)" }
        ]
      },
      {
        "name": "Ethnic",
        "instruments": [
          { "code": 104, "name": "Sitar" },
          { "code": 105, "name": "Banjo" },
          { "code": 106, "name": "Shamisen" },
          { "code": 107, "name": "Koto" },
          { "code": 108, "name": "Kalimba" },
          { "code": 109, "name": "Bag pipe" },
          { "code": 110, "name": "Fiddle" },
          { "code": 111, "name": "Shanai" }
        ]
      },
      {
        "name": "Percussive",
        "instruments": [
          { "code": 112, "name": "Tinkle Bell" },
          { "code": 113, "name": "Cowbell" },
          { "code": 114, "name": "Steel Drums" },
          { "code": 115, "name": "Woodblock" },
          { "code": 116, "name": "Taiko Drum" },
          { "code": 117, "name": "Melodic Tom" },
          { "code": 118, "name": "Synth Drum" },
          { "code": 119, "name": "Reverse Cymbal" }
        ]
      },
      {
        "name": "Sound Effects",
        "instruments": [
          { "code": 120, "name": "Guitar Fret Noise" },
          { "code": 121, "name": "Breath Noise" },
          { "code": 122, "name": "Seashore" },
          { "code": 123, "name": "Bird Tweet" },
          { "code": 124, "name": "Telephone Ring" },
          { "code": 125, "name": "Helicopter" },
          { "code": 126, "name": "Applause" },
          { "code": 127, "name": "Gunshot" }
        ]
      }
    ]
  }

  ScrollView {
    id: scrollView
    anchors.fill: parent

    GridLayout {
        id: gridLayout
        columns: 3
        width: scrollView.width

        Repeater {
            model: instrumentData.categories

            ColumnLayout {
                Layout.fillWidth: true

                Text {
                    text: modelData.name
                    font.italic: true
                    font.pixelSize: 18
                    padding: 10
                    color: rootPane.palette.text
                }

                ListView {
                    model: modelData.instruments
                    Layout.fillWidth: true
                    height: childrenRect.height
                    spacing: 10

                    delegate: RowLayout {
                        width: parent.width

                        Text {
                            text: modelData.code
                            rightPadding: 10
                            leftPadding: 10
                            color: rootPane.palette.text
                        }
                        Text {
                            text: modelData.name
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            color: rootPane.palette.text
                        }
                    }
                }
            }
        }
    }
  }
}
