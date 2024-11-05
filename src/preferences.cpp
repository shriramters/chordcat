// SPDX-License-Identifier: GPL-3.0-only
#include "preferences.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

void Preferences::load(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        throw SettingsFileDoesntExistException();
    }

    nlohmann::json settingsJson = nlohmann::json::parse(ifs);
    this->piano.gain = settingsJson["piano"]["gain"];
    this->piano.pressed_note_colors[0] = settingsJson["piano"]["pressed_note_colors"]["r"];
    this->piano.pressed_note_colors[1] = settingsJson["piano"]["pressed_note_colors"]["g"];
    this->piano.pressed_note_colors[2] = settingsJson["piano"]["pressed_note_colors"]["b"];
    this->piano.pressed_note_colors[3] = settingsJson["piano"]["pressed_note_colors"]["a"];
    this->ui.font.name = settingsJson["ui"]["font"]["name"];
    this->ui.font.path = settingsJson["ui"]["font"]["path"];
}

void Preferences::save(const std::string& filename) {
    nlohmann::json settingsJson;

    settingsJson["piano"]["gain"] = this->piano.gain;
    settingsJson["piano"]["pressed_note_colors"]["r"] = this->piano.pressed_note_colors[0];
    settingsJson["piano"]["pressed_note_colors"]["g"] = this->piano.pressed_note_colors[1];
    settingsJson["piano"]["pressed_note_colors"]["b"] = this->piano.pressed_note_colors[2];
    settingsJson["piano"]["pressed_note_colors"]["a"] = this->piano.pressed_note_colors[3];
    settingsJson["ui"]["font"]["name"] = this->ui.font.name;
    settingsJson["ui"]["font"]["path"] = this->ui.font.path;

    std::ofstream ofs(filename);
    ofs << settingsJson.dump(4);
}
