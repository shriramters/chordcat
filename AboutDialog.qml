import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: aboutDialog
    title: "About Chordcat"
    modal: true

    property string version: "1.0.0-rc1"
    property string author: "Shriram Ravindranathan"
    property string license: "GPL-3.0-only"
    property string website: "https://shriramters.github.io/chordcat/"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Image {
            source: "qrc:/assets/images/chordcat.png"
            Layout.alignment: Qt.AlignHCenter
            sourceSize.width: 128
            sourceSize.height: 128
            fillMode: Image.PreserveAspectFit
        }

        Label {
            text: "Chordcat v" + aboutDialog.version
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "A chord naming application."
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "© 2024-2025 " + aboutDialog.author
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "License: " + aboutDialog.license
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: "Website: <a href=\"" + aboutDialog.website + "\">" + aboutDialog.website + "</a>"
            Layout.alignment: Qt.AlignHCenter
            onLinkActivated: Qt.openUrlExternally(link)
        }

        GroupBox {
            title: "Dependencies"
            Layout.fillWidth: true

            ColumnLayout {
                Label { text: "Qt Version: " + qtVersion }
                Label { id: fluidsynthLabel; text: "FluidSynth Version: " + fluidsynthVersion }
                Label { id: libremidiLabel; text: "Libremidi Version: " + libremidiVersion }
            }
        }
    }
}
