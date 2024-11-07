#include "sound_font_manager.hpp"
#include "config.h"

SoundFontManager::SoundFontManager() {
    systemDir = std::filesystem::path(std::string(APP_ASSETS_PATH) + "/soundfonts/");
    userDir = getUserSoundFontDir();

    if (userDir) {
        std::filesystem::create_directories(*userDir);
    }
}

std::vector<std::pair<std::string, std::filesystem::path>>
SoundFontManager::getAvailableSoundFonts() const {
    std::vector<std::pair<std::string, std::filesystem::path>> soundfonts;

    // search system dir
    for (const auto& entry : std::filesystem::directory_iterator(systemDir)) {
        if (entry.path().extension() == ".sf2") {
            soundfonts.emplace_back(entry.path().filename().string(), entry.path());
        }
    }

    // search user dir if available
    if (userDir) {
        for (const auto& entry : std::filesystem::directory_iterator(*userDir)) {
            if (entry.path().extension() == ".sf2") {
                soundfonts.emplace_back(entry.path().filename().string(), entry.path());
            }
        }
    }

    return soundfonts;
}
std::optional<std::filesystem::path> SoundFontManager::getUserSoundFontDir() const {
#ifdef _WIN32
    char* appDataRoot = std::getenv("APPDATA");
    if (appDataRoot == nullptr) {
        std::cerr << "Can't find user data directory, env variable $APPDATA was NULL";
        return std::nullopt;
    }
    return std::filesystem::path(appDataRoot) / "chordcat" / "soundfonts";
#else
    char* dataRoot = std::getenv("XDG_DATA_HOME");
    if (dataRoot == nullptr) {
        dataRoot = std::getenv("HOME");
        if (dataRoot != nullptr) {
            return std::filesystem::path(dataRoot) / ".local" / "share" / "chordcat" / "soundfonts";
        } else {
            std::cerr << "Can't find user data directory, env variable $HOME and "
                         "$XDG_DATA_HOME were NULL";
            return std::nullopt;
        }
    }
    return std::filesystem::path(dataRoot) / "chordcat" / "soundfonts";
#endif
}
