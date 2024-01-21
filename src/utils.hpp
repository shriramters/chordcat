#pragma once
#include <string>
#include <vector>

const std::vector<std::string> note_names = { "A","Bb","B","C","C#","D","Eb","E","F","F#","G","Ab" };

std::vector<std::string> key_numbers_to_note_names(const std::vector<size_t>& indices) {
    std::vector<std::string> result = {};
    for (auto index : indices) {
        result.push_back(note_names[index % note_names.size()]);
    }
    return result;
}