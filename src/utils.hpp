#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include <algorithm>

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

std::unordered_map<std::string, std::string> chord_names = {
    {"1","minor second"},
    {"2","major second"},
    {"3","minor third"},
    {"4","major third"},
    {"5","perfect fourth"},
    {"6","tritone"},
    {"7","perfect fifth"},
    {"8","minor sixth"},
    {"9","major sixth"},
    {"10","minor seventh"},
    {"11","major seventh"},
    {"12","octave"},
    {"13","minor ninth"},
    {"14","major ninth"},
    {"24","octave"},
    {"2,7","sus2"},
    {"3,7","minor"},
    {"4,7","major"},
    {"5,7","sus4"},
    {"3,7","minor"},
    {"3,7,8","minb6"},
    {"4,5,9","sus4(no5)add6"},
    {"5,8,1","sus4(no5)(no3)b2b6"},
    {"4,7,11","maj7"},
    {"3,7,10","min7"},
    {"4,7,10","7"},
    {"3,7,11","minMaj7"},
    {"3,6,10"," half diminished"},
    {"2,4,5,7,9,11"," major scale"},
    {"2,3,5,7,9,10"," dorian scale"},
    {"1,3,5,7,8,10"," phrygian scale"},
    {"2,4,6,7,9,11"," lydian scale"},
    {"2,4,5,7,9,10"," mixolydian scale"},
    {"2,3,5,7,8,10"," aeolian scale"},
    {"1,3,5,6,8,10"," locrian scale"},
};

std::vector<std::string> name_that_chord(const std::vector<size_t>& indices)
{
    std::set<unsigned short> notes = {};
    for (auto index : indices) {
        notes.insert(index);
    }
    std::vector<std::string> result = {};
    for (auto root : notes) {
        std::vector<unsigned short> intervals = {};
        for (auto other : notes) {
            if (other == root) continue;
            intervals.push_back(get_note_distance(root, other));
        }
        // std::cout << note_names[root%12] << " ";
        // for (auto i : intervals) {
        //     std::cout << i << " ";
        // }
        // std::cout << std::endl;
        std::string search_term = "";
        std::sort(intervals.begin(), intervals.end());
        for (auto i : intervals) {
            search_term += std::to_string(i) + ",";
        }
        if (!search_term.empty())search_term.pop_back();
        if (chord_names.contains(search_term)) {
            result.push_back((intervals.size() > 1) ? (note_names[root % 12] + chord_names[search_term]) : chord_names[search_term]);
        }
    }
    return result;
}