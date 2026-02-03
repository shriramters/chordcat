#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QAudioFormat>
#include <QAudioSink>
#include <QQmlContext>
#include <fluidsynth.h>
#include <fluidsynth/version.h>
#include <libremidi/config.hpp>
#include "piano.hpp"
#include "grand_staff.hpp"
#include "chord_identifier.hpp"
#include "sound_font_manager.hpp"
#include "midi_manager.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("Ters");
    app.setApplicationName("Chordcat");

    qRegisterMetaType<std::vector<size_t>>("std::vector<size_t>");

    // Services
    Piano piano;
    ChordIdentifier chordIdentifier;
    SoundFontManager sfManager;
    MidiManager midiManager;

    // Connections
    QObject::connect(&midiManager, &MidiManager::midiMessageReceived, &piano, &Piano::midiEvent);
    QObject::connect(&piano, &Piano::pressedNotesChanged,
                     &chordIdentifier, &ChordIdentifier::updatePressedNotes);

    qmlRegisterType<GrandStaff>("Chordcat", 1, 0, "GrandStaff");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("pianoBackend", &piano);
    engine.rootContext()->setContextProperty("chordIdentifier", &chordIdentifier);
    engine.rootContext()->setContextProperty("soundFontManager", &sfManager);
    engine.rootContext()->setContextProperty("midiManager", &midiManager);

    // Version Info
    engine.rootContext()->setContextProperty("qtVersion", QString(qVersion()));
    engine.rootContext()->setContextProperty("fluidsynthVersion", QString(FLUIDSYNTH_VERSION));
    engine.rootContext()->setContextProperty("libremidiVersion", QString(LIBREMIDI_VERSION));

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Chordcat", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
