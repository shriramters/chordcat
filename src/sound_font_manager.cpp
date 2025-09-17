// SPDX-License-Identifier: GPL-3.0-only
#include "sound_font_manager.hpp"
#include <QDirIterator>
#include <QStandardPaths>
#include <QDebug>

SoundFontManager::SoundFontManager(QObject* parent) : QObject(parent) {
    findSoundFonts();
}

QStringList SoundFontManager::availableSoundFonts() const {
    return m_soundfont_names;
}

QString SoundFontManager::getSoundFontPath(const QString& name) const
{
    auto it = std::find_if(m_soundfonts.begin(), m_soundfonts.end(),
                           [&](const auto& pair) {
                               return QString::fromStdString(pair.first) == name;
                           });
    if (it != m_soundfonts.end()) {
        return QString::fromStdString(it->second.string());
    }
    return QString();
}

void SoundFontManager::findSoundFonts() {
    m_soundfonts.clear();
    m_soundfont_names.clear();

    QStringList searchPaths;

    // User-specific writable data location
    searchPaths.append(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/soundfonts");

    // System-wide read-only data locations
    for (const QString& dataDir : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        searchPaths.append(dataDir + "/chordcat/assets/soundfonts");
    }
    searchPaths.append("/usr/share/soundfonts");

    // Ensure user directory exists (only for the writable path)
    std::filesystem::path userDir = getUserSoundFontDir();
    if (!std::filesystem::exists(userDir)) {
        std::filesystem::create_directories(userDir);
    }

    // Iterate through all collected search paths
    for (const QString& path : searchPaths) {
        QDirIterator it(path, {"*.sf2"}, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString filePath = it.next();
            QString name = QFileInfo(filePath).fileName();
            if (!m_soundfont_names.contains(name)) {
                m_soundfonts.emplace_back(name.toStdString(), std::filesystem::path(filePath.toStdString()));
                m_soundfont_names.append(name);
            }
        }
    }

    emit availableSoundFontsChanged();
}

std::filesystem::path SoundFontManager::getUserSoundFontDir() const {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    return std::filesystem::path(dataPath.toStdString()) / "soundfonts";
}
