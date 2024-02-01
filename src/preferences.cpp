#include "preferences.hpp"
#include <exception>
#include <iostream>
namespace pt = boost::property_tree;

void Preferences::load(const std::string& filename)
{
    pt::read_xml(filename, tree);
    this->piano.gain = tree.get<float>("piano.gain");
    this->piano.pressed_note_colors[0] = tree.get<float>("piano.pressed_note_colors.r");
    this->piano.pressed_note_colors[1] = tree.get<float>("piano.pressed_note_colors.g");
    this->piano.pressed_note_colors[2] = tree.get<float>("piano.pressed_note_colors.b");
    this->piano.pressed_note_colors[3] = tree.get<float>("piano.pressed_note_colors.a");
}

void Preferences::save(const std::string& filename)
{
    tree.put("piano.gain", piano.gain);
    tree.put("piano.pressed_note_colors.r", piano.pressed_note_colors[0]);
    tree.put("piano.pressed_note_colors.g", piano.pressed_note_colors[1]);
    tree.put("piano.pressed_note_colors.b", piano.pressed_note_colors[2]);
    tree.put("piano.pressed_note_colors.a", piano.pressed_note_colors[3]);
    pt::write_xml(filename, tree);
}