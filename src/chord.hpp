// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <array>
#include <string>
#include <vector>

const std::array<std::string, 12> degrees = { "root", "b2", "2",  "b3", "3",  "4",
                                             "b5",   "5",  "b6", "6",  "b7", "7" };
const std::array<std::string, 12> compound_tones = { "octave", "b9", "9",   "b10", "10",  "11",
                                                    "#11",    "5",  "b13", "13",  "b7", "7" };

inline std::string key_number_to_note_name(const size_t index) {
    static const std::array<std::string, 12> note_names = { "A",  "Bb", "B", "C",  "C#", "D",
                                                           "Eb", "E",  "F", "F#", "G",  "Ab" };
    return note_names[index % note_names.size()];
}

struct Chord {
    unsigned short root;
    std::string base_name;
    std::vector<unsigned short> extra_tones;
    std::vector<unsigned short> omitted_tones;
    unsigned num_accidentals;

    // support sorting
    friend auto operator<=>(Chord const& a, Chord const& b) {
        return a.num_accidentals <=> b.num_accidentals;
    }

    // to string
    std::string to_string() {
        std::string res = key_number_to_note_name(root) + base_name;
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
