#pragma once
#include <string>
#include <vector>
#include "utils.hpp"

const std::array<std::string, 12> degrees = { "root","b9","9","b3","3","11","#11","5","b13","13","b7","7" };

inline std::string key_number_to_note_name(const size_t index) {
    static const std::array<std::string, 12> note_names =
    { "A","Bb","B","C","C#","D","Eb","E","F","F#","G","Ab" };
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
        for (auto tone : omitted_tones)
            res += "(no" + degrees[tone % 12] + ")";
        for (auto tone : extra_tones)
            res += "/" + degrees[tone % 12];
        return res;
    }
};