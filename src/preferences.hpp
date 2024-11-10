// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "config.h"
#include <array>
#include <string>
#include <vector>

// TODO: std::filesystem Path
const std::vector<std::pair<std::string, std::string>> available_fonts = {
    {"Metropolis", std::string(APP_ASSETS_PATH) + "/fonts/Metropolis/Metropolis-Regular.otf"},
    {"DejaVu Sans", std::string(APP_ASSETS_PATH) + "/fonts/DejaVuSans/ttf/DejaVuSans.ttf"},
    {"FirstTimeWriting!",
     std::string(APP_ASSETS_PATH) + "/fonts/FirstTimeWriting/FirstTimeWriting!.ttf"},
    {"Petaluma", std::string(APP_ASSETS_PATH) + "/fonts/Petaluma/otf/PetalumaScript.otf"}};

struct Preferences {
    struct Piano {
        float gain;
        std::array<float, 4> pressed_note_colors;
        Piano() {
            gain = 1.f;
            pressed_note_colors = {0.945f, 0.275f, 0.275f, 1.f};
        }
    };
    struct UI {
        struct Font {
            std::string name;
            std::string path;
            Font() {
                name = available_fonts[0].first;
                path = available_fonts[0].second;
            }
        };
        Font font;
        UI() {}
    };

    UI ui;
    Piano piano;
    bool setup();
    void load(const std::string& filename);
    bool save();
};
