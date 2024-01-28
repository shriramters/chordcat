// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <ranges>
#include "chord_db.hpp"
#include "chord.hpp"

const std::vector<std::string> note_names = { "A","Bb","B","C","C#","D","Eb","E","F","F#","G","Ab" };

std::vector<std::string> key_numbers_to_note_names(const std::vector<size_t>& indices) {
    std::vector<std::string> result = {};
    for (auto index : indices) {
        result.push_back(note_names[index % note_names.size()]);
    }
    return result;
}

inline unsigned short get_note_distance(unsigned short root, unsigned short other) {
    if (root >= other) {
        return 12 + (other - root) % 12;
    }
    return other - root;
}

void insert_chords(const unsigned short root, const std::set<unsigned short>& intervals, std::multiset<Chord>& res) {
    std::set<Chord> temp;
    for (auto& [name, notes] : chord_db) {
        Chord chord = {};
        chord.root = root;
        chord.base_name = name;

        //TODO: Rewite using std::ranges::set_difference
        std::set_difference(notes.begin(), notes.end(),
            intervals.begin(), intervals.end(),
            std::back_inserter(chord.omitted_tones));
        std::set_difference(intervals.begin(), intervals.end(),
            notes.begin(), notes.end(),
            std::back_inserter(chord.extra_tones));
        chord.num_accidentals = chord.extra_tones.size() + chord.omitted_tones.size();

        temp.insert(chord);
        // if (chord.num_accidentals <= intervals.size())
        //     res.insert(chord);
    }
    if (!temp.empty())
        res.insert(*temp.begin());
}

std::multiset<Chord> name_that_chord(const std::vector<size_t>& indices)
{
    std::set<unsigned short> notes = {};
    for (auto index : indices) {
        notes.insert(index % 12);
    }
    std::multiset<Chord> result = {};
    for (auto root : notes) {
        std::set<unsigned short> intervals = {};
        for (auto other : notes) {
            if (other == root) continue;
            intervals.insert(get_note_distance(root, other));
        }
        insert_chords(root, intervals, result);
    }
    return result;
}