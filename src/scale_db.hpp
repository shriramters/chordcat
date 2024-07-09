// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <SFML/System/String.hpp>
#include <set>
#include <vector>

// TODO: constexpr in future
const std::vector<std::pair<const sf::String, std::set<unsigned short>>> scale_db = {
    {"Major Scale", {2, 4, 5, 7, 9, 11}},
    {"Dorian Scale", {2, 3, 5, 7, 9, 10}},
    {"Phrygian Scale", {1, 3, 5, 7, 8, 10}},
    {"Lydian Scale", {2, 4, 6, 7, 9, 11}},
    {"Mixolydian Scale", {2, 4, 5, 7, 9, 10}},
    {"Aeolian Scale", {2, 3, 5, 7, 8, 10}},
    {"Locrian Scale", {1, 3, 5, 6, 8, 10}},
    {"Harmonic Minor Scale", {2, 3, 5, 7, 8, 11}},
    {"Locrian Natural 6 Scale", {1, 3, 5, 6, 9, 10}},
    {"Augmented Major Scale", {2, 4, 5, 8, 9, 11}},
    {L"Dorian ♯11 Scale", {2, 3, 6, 7, 9, 10}},
    {"Phrygian Dominant Scale", {1, 4, 5, 7, 8, 10}},
    {L"Lydian ♯2 Scale", {3, 4, 6, 7, 9, 11}},
    {L"Super Locrian ♭♭7 Scale", {1, 3, 4, 6, 8, 9}},
    {"Jazz Minor Scale", {2, 3, 5, 7, 9, 11}},
    {L"Dorian ♭2 Scale", {1, 3, 5, 7, 9, 10}},
    {"Lydian Augmented Scale", {2, 4, 6, 8, 9, 11}},
    {"Lydian Dominant Scale", {2, 4, 6, 7, 9, 10}},
    {"Aeolian Dominant Scale", {2, 4, 5, 7, 8, 10}},
    {"Half-diminished Scale", {2, 3, 5, 6, 8, 10}},
    {"Altered Scale", {1, 3, 4, 6, 8, 10}}
};
