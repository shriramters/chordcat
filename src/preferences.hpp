// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "config.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <CoreFoundation/CFBundle.h>

std::string get_resources_dir();

// TODO: std::filesystem Path
const std::vector<std::pair<std::string, std::string>> available_fonts = {
    {"DejaVu Sans", get_resources_dir() + "/assets/fonts/DejaVuSans/ttf/DejaVuSans.ttf"},
    {"FirstTimeWriting!",
     get_resources_dir() + "/assets/fonts/FirstTimeWriting/FirstTimeWriting!.ttf"},
    {"Petaluma", get_resources_dir() + "/assets/fonts/Petaluma/otf/PetalumaScript.otf"} };

namespace pt = boost::property_tree;

struct Preferences {
    struct Piano {
        float gain;
        std::array<float, 4> pressed_note_colors;
        Piano() {
            gain = 2.f;
            pressed_note_colors = { 0.9f, 0.7f, 0.f, 1.f };
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
    pt::ptree tree;
    void load(const std::string& filename);
    void save(const std::string& filename);
};
