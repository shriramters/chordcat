// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "key.hpp"
#include <SFML/System/String.hpp>
#include <array>
#include <vector>

const std::array<sf::String, 12> degrees = {"root", L"♭2", "2",   L"♭3", "3",   "4",
                                            L"♭5",  "5",   L"♭6", "6",   L"♭7", "7"};
const std::array<sf::String, 12> compound_tones = {"octave", L"♭9", "9",    L"♭10", "10",  "11",
                                                   L"♯11",   "5",   L"♭13", "13",   L"♭7", "7"};

static const std::array<sf::String, 12> sharp_names = {"A",   L"A♯", "B", "C",   L"C♯", "D",
                                                       L"D♯", "E",   "F", L"F♯", "G",   L"G♯"};

static const std::array<sf::String, 12> flat_names = {"A",   L"B♭", "B", "C",   L"D♭", "D",
                                                      L"E♭", "E",   "F", L"G♭", "G",   L"A♭"};

inline sf::String key_number_to_note_name(std::size_t index, Key key) {
    if (is_sharp_key(key)) {
        return sharp_names[index % 12];
    } else {
        return flat_names[index % 12];
    }
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
    sf::String to_sf_string(Key k) {
        sf::String res = key_number_to_note_name(root, k) + base_name;
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
