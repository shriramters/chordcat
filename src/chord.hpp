// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <array>
#include <string>
#include <vector>

const std::array<sf::String, 12> degrees = { "root", L"♭2", "2",  L"♭3", "3",  "4",
                                             L"♭5",   "5",  L"♭6", "6",  L"♭7", "7" };
const std::array<sf::String, 12> compound_tones = { "octave", L"♭9", "9",   L"♭10", "10",  "11",
                                                    L"♯11",    "5",  L"♭13", "13",  L"♭7", "7" };

inline sf::String key_number_to_note_name(const size_t index) {
    static const std::array<sf::String, 12> note_names = { "A",  L"B♭", "B", "C",  L"C♯", "D",
                                                           L"E♭", "E",  "F", L"F♯", "G",  L"A♭" };
    return note_names[index % note_names.size()];
}

struct Chord {
    unsigned short root;
    sf::String base_name;
    std::vector<unsigned short> extra_tones;
    std::vector<unsigned short> omitted_tones;
    unsigned num_accidentals;

    // support sorting
    friend auto operator<=>(Chord const& a, Chord const& b) {
        return a.num_accidentals <=> b.num_accidentals;
    }

    // to string
    sf::String to_sf_string() {
        sf::String res = key_number_to_note_name(root) + base_name;
        unsigned accidentals_count = num_accidentals;
        if (accidentals_count > 0) {
            res += "(";
        }

        for (auto tone : omitted_tones)
            res += "no" + degrees[tone % 12] + (--num_accidentals == 0 ? ")" : ",");
        for (auto tone : extra_tones)
            res += compound_tones[tone % 12] + (--num_accidentals == 0 ? ")" : ",");
        return res;
    }
};
