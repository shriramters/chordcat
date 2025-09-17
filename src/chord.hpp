// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "key.hpp"
#include <QString>
#include <string>
#include <array>
#include <vector>

const std::array<std::wstring, 12> degrees_w = {L"root", L"♭2", L"2",   L"♭3", L"3",   L"4",
                                            L"♭5",  L"5",   L"♭6", L"6",   L"♭7", L"7"};
const std::array<std::wstring, 12> compound_tones_w = {L"octave", L"♭9", L"9",    L"♭10", L"10",  L"11",
                                                   L"♯11",   L"5",   L"♭13", L"13",   L"♭7", L"7"};

static const std::array<std::wstring, 12> sharp_names_w = {L"A",   L"A♯", L"B", L"C",   L"C♯", L"D",
                                                       L"D♯", L"E",   L"F", L"F♯", L"G",   L"G♯"};

static const std::array<std::wstring, 12> flat_names_w = {L"A",   L"B♭", L"B", L"C",   L"D♭", L"D",
                                                      L"E♭", L"E",   L"F", L"G♭", L"G",   L"A♭"};

inline QString key_number_to_note_name(std::size_t index, Key key) {
    if (is_sharp_key(key)) {
        return QString::fromStdWString(sharp_names_w[index % 12]);
    } else {
        return QString::fromStdWString(flat_names_w[index % 12]);
    }
}

struct Chord {
    unsigned short root;
    QString base_name;
    std::vector<unsigned short> extra_tones;
    std::vector<unsigned short> omitted_tones;
    unsigned num_accidentals;

    // support sorting
    friend auto operator<=>(Chord const& a, Chord const& b) {
        return a.num_accidentals <=> b.num_accidentals;
    }

    // to string
    QString toString(Key k) const {
        QString res = key_number_to_note_name(root, k) + base_name;
        if (num_accidentals == 0) {
            return res;
        }
        res += "(";
        bool first = true;
        for (auto tone : omitted_tones) {
            if (!first)
                res += ",";
            res += "no" + QString::fromStdWString(degrees_w[tone % 12]);
            first = false;
        }
        for (auto tone : extra_tones) {
            if (!first)
                res += ",";
            res += QString::fromStdWString(compound_tones_w[tone % 12]);
            first = false;
        }
        res += ")";
        return res;
    }
};
