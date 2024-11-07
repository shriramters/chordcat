// SPDX-License-Identifier: GPL-3.0-only
#include "preferences.hpp"
#include "utils.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

constexpr std::string_view settings_file_name = "settings.json";

std::optional<std::string> get_settings_file_path() {
    std::optional<std::string> appdata_path = get_appdata_path();
    if (appdata_path.has_value()) {
        return *appdata_path + "/" + std::string(settings_file_name);
    }
    return std::nullopt;
}

void Preferences::load(const std::string& filepath) {
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        throw std::runtime_error("settings file doesn't exist");
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

bool Preferences::save() {
    std::optional<std::string> filepath = get_settings_file_path();
    if (filepath.has_value()) {
        nlohmann::json settingsJson;

        settingsJson["piano"]["gain"] = this->piano.gain;
        settingsJson["piano"]["pressed_note_colors"]["r"] = this->piano.pressed_note_colors[0];
        settingsJson["piano"]["pressed_note_colors"]["g"] = this->piano.pressed_note_colors[1];
        settingsJson["piano"]["pressed_note_colors"]["b"] = this->piano.pressed_note_colors[2];
        settingsJson["piano"]["pressed_note_colors"]["a"] = this->piano.pressed_note_colors[3];
        settingsJson["ui"]["font"]["name"] = this->ui.font.name;
        settingsJson["ui"]["font"]["path"] = this->ui.font.path;

        std::ofstream ofs(*filepath);
        ofs << settingsJson.dump(4);
    } else {
        return false;
    }

    return true;
}

bool Preferences::setup() {
    std::optional<std::string> filepath = get_settings_file_path();
    if (filepath.has_value()) {
        try {
            this->load(*filepath);
        } catch (std::runtime_error e) {
            std::cout << e.what() << std::endl;
            std::error_code err;
            if (!CreateDirectoryRecursive(*get_appdata_path(), err)) {
                std::cerr << "CreateDirectoryRecursive FAILED, err: " << err.message() << std::endl;
                return false;
            }
            this->save();
        } catch (nlohmann::json::parse_error) {
            // if there is a parsing error file was tampered improperly
            // reset the settings file with sensible defaults
            this->save();
        }
    } else {
        return false;
    }

    return true;
}
