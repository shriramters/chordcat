// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <SFML/System/String.hpp>
#include <array>
#include <vector>

const std::array<sf::String, 12> degrees = {"root", L"♭2", "2",   L"♭3", "3",   "4",
                                            L"♭5",  "5",   L"♭6", "6",   L"♭7", "7"};
const std::array<sf::String, 12> compound_tones = {"octave", L"♭9", "9",    L"♭10", "10",  "11",
                                                   L"♯11",   "5",   L"♭13", "13",   L"♭7", "7"};

enum class Key {
    CMajor,
    GMajor,
    DMajor,
    AMajor,
    EMajor,
    BMajor,
    FSharpMajor, // enharmonic with G♭ major
    CSharpMajor, // enharmonic with D♭ major
    FMajor,
    BbMajor, // enharmonic with A♯ major
    EbMajor, // enharmonic with D♯ major
    AbMajor, // enharmonic with G♯ major

    // Enharmonic keys:
    DbMajor,     // enharmonic with C♯ major
    GbMajor,     // enharmonic with F♯ major
    CbMajor,     // enharmonic with B major
    DSharpMajor, // enharmonic with E♭ major
    GSharpMajor, // enharmonic with A♭ major
    ASharpMajor  // enharmonic with B♭ major
};

inline bool is_sharp_key(Key key) {
    switch (key) {
    case Key::CMajor:
    case Key::FMajor:
    case Key::BbMajor:
    case Key::EbMajor:
    case Key::AbMajor:
    case Key::DbMajor:
    case Key::GbMajor:
    case Key::CbMajor:
        return false;
    default:
        return true;
    }
}

inline bool is_flat_key(Key key) { return !is_sharp_key(key); }

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
