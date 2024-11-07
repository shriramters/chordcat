#pragma once
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

class SoundFontManager {
  public:
    SoundFontManager();

    std::vector<std::pair<std::string, std::filesystem::path>> getAvailableSoundFonts() const;

  private:
    std::filesystem::path systemDir;
    std::optional<std::filesystem::path> userDir;

    std::optional<std::filesystem::path> getUserSoundFontDir() const;
};
