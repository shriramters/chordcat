// SPDX-License-Identifier: GPL-3.0-only
#include "midi_manager.hpp"
#include "utils.hpp" // For to_string_view
#include <QDebug>
#include <libremidi/config.hpp>
#include <libremidi/libremidi.hpp>

// Register MidiEvent so it can be used in queued connections
Q_DECLARE_METATYPE(MidiEvent);

MidiManager::MidiManager(QObject* parent) : QObject(parent) {
    qRegisterMetaType<MidiEvent>();

    m_backendApis = libremidi::available_apis();
    for (const auto& api : m_backendApis) {
        m_availableBackends.append(QString::fromStdString(std::string(to_string_view(api))));
    }

    if (!m_backendApis.empty()) {
        m_currentApi = m_backendApis[0];
    }

    createMidiIn();
}

MidiManager::~MidiManager() = default;

QStringList MidiManager::availableBackends() const { return m_availableBackends; }

QStringList MidiManager::availablePorts() const { return m_availablePorts; }

int MidiManager::currentBackendIndex() const {
    auto it = std::find(m_backendApis.begin(), m_backendApis.end(), m_currentApi);
    if (it != m_backendApis.end()) {
        return std::distance(m_backendApis.begin(), it);
    }
    return 0;
}

int MidiManager::currentPortIndex() const { return m_currentPortIndex; }

void MidiManager::setCurrentBackendIndex(int index) {
    if (index >= 0 && index < m_backendApis.size()) {
        if (m_backendApis[index] != m_currentApi) {
            m_currentApi = m_backendApis[index];
            createMidiIn();
            emit currentBackendChanged();
        }
    }
}

void MidiManager::setCurrentPortIndex(int index) {
    if (m_midi_in && index >= 0 && index < m_ports.size()) {
        m_midi_in->close_port();
        try {
            m_midi_in->open_port(m_ports[index]);
            m_currentPortIndex = index;
            qDebug() << "Opened MIDI input port:" << m_availablePorts[index];
            emit portChanged();
        } catch (const libremidi::midi_exception& e) {
            qWarning() << "Failed to open MIDI port: " << e.what();
            m_currentPortIndex = -1;
            emit portChanged();
        }
    } else {
        if (m_midi_in)
            m_midi_in->close_port();
        m_currentPortIndex = -1;
        emit portChanged();
    }
}

void MidiManager::refreshPorts() {
    m_availablePorts.clear();
    m_ports.clear();
    try {
        m_ports = libremidi::observer{{}, observer_configuration_for(m_midi_in->get_current_api())}
                         .get_input_ports();
        for (const auto& port : m_ports) {
            m_availablePorts.append(QString::fromStdString(port.display_name));
        }
    } catch (const libremidi::midi_exception& e) {
        qWarning() << "Failed to get MIDI ports: " << e.what();
    }

    emit portsChanged();
}

void MidiManager::createMidiIn() {
    try {
        m_midi_in = std::make_unique<libremidi::midi_in>(
            libremidi::input_configuration{
                .on_message = [this](const libremidi::message& msg) {
                    if (msg.size() > 0) {
                        emit midiMessageReceived(MidiEvent(msg));
                    }
                }},
            libremidi::midi_in_configuration_for(m_currentApi));

        refreshPorts();
        // Try to open the first port by default
        if (!m_availablePorts.isEmpty()) {
            setCurrentPortIndex(0);
        }
    } catch (const libremidi::midi_exception& e) {
        qWarning() << "Failed to create MIDI input: " << e.what();
        m_midi_in.reset();
        refreshPorts(); // clear ports
    }
}
