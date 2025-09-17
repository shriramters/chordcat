// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include "midi_event.hpp"
#include <QObject>
#include <QStringList>
#include <libremidi/libremidi.hpp>
#include <memory>

class MidiManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableBackends READ availableBackends CONSTANT)
    Q_PROPERTY(QStringList availablePorts READ availablePorts NOTIFY portsChanged)
    Q_PROPERTY(int currentBackendIndex READ currentBackendIndex WRITE
                   setCurrentBackendIndex NOTIFY currentBackendChanged)
    Q_PROPERTY(int currentPortIndex READ currentPortIndex WRITE setCurrentPortIndex NOTIFY
                   portChanged)

  public:
    explicit MidiManager(QObject* parent = nullptr);
    ~MidiManager();

    QStringList availableBackends() const;
    QStringList availablePorts() const;
    int currentBackendIndex() const;
    int currentPortIndex() const;

  public slots:
    void setCurrentBackendIndex(int index);
    void setCurrentPortIndex(int index);

  signals:
    void portsChanged();
    void currentBackendChanged();
    void portChanged();
    void midiMessageReceived(MidiEvent event);

  private:
    void refreshPorts();
    void createMidiIn();

    std::unique_ptr<libremidi::midi_in> m_midi_in;
    libremidi::API m_currentApi;
    int m_currentPortIndex = -1;

    QStringList m_availablePorts;
    QStringList m_availableBackends;
    std::vector<libremidi::API> m_backendApis;
    std::vector<libremidi::input_port> m_ports;
};
