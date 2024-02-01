#pragma once
#include <string>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

struct Preferences
{
    struct Piano {
        float gain;
        std::array<float, 4> pressed_note_colors;
        Piano() {
            gain = 2.f;
            pressed_note_colors = { 0.9f,0.7f,0.f, 1.f };
        }
    };
    Piano piano;
    pt::ptree tree;
    void load(const std::string& filename);
    void save(const std::string& filename);
};

