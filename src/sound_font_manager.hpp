// SPDX-License-Identifier: GPL-3.0-only
#pragma once

#include <QObject>
#include <QStringList>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class SoundFontManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QStringList availableSoundFonts READ availableSoundFonts NOTIFY
                   availableSoundFontsChanged)

  public:
    explicit SoundFontManager(QObject* parent = nullptr);

    QStringList availableSoundFonts() const;

    Q_INVOKABLE QString getSoundFontPath(const QString& name) const;

  signals:
    void availableSoundFontsChanged();

  private:
    void findSoundFonts();
    std::filesystem::path getUserSoundFontDir() const;

    std::vector<std::pair<std::string, std::filesystem::path>> m_soundfonts;
    QStringList m_soundfont_names;
};
