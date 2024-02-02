#include "preferences.hpp"

namespace pt = boost::property_tree;

void Preferences::load(const std::string& filename) {
    pt::read_xml(filename, tree);
    this->piano.gain = tree.get<float>("piano.gain");
    this->ui.font.name = tree.get<std::string>("ui.font.name");
    this->ui.font.path = tree.get<std::string>("ui.font.path");
    this->piano.pressed_note_colors[0] = tree.get<float>("piano.pressed_note_colors.r");
    this->piano.pressed_note_colors[1] = tree.get<float>("piano.pressed_note_colors.g");
    this->piano.pressed_note_colors[2] = tree.get<float>("piano.pressed_note_colors.b");
    this->piano.pressed_note_colors[3] = tree.get<float>("piano.pressed_note_colors.a");
}

void Preferences::save(const std::string& filename) {
    tree.put("piano.gain", piano.gain);
    tree.put("ui.font.name", ui.font.name);
    tree.put("ui.font.path", ui.font.path);
    tree.put("piano.pressed_note_colors.r", piano.pressed_note_colors[0]);
    tree.put("piano.pressed_note_colors.g", piano.pressed_note_colors[1]);
    tree.put("piano.pressed_note_colors.b", piano.pressed_note_colors[2]);
    tree.put("piano.pressed_note_colors.a", piano.pressed_note_colors[3]);
    pt::write_xml(filename, tree);
}
