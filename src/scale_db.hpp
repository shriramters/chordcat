// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <string>
#include <set>
#include <vector>

// TODO: constexpr in future
const std::vector<std::pair<const std::wstring, std::set<unsigned short>>> scale_db = {
    {L"Major Scale", {2, 4, 5, 7, 9, 11}},
    {L"Dorian Scale", {2, 3, 5, 7, 9, 10}},
    {L"Phrygian Scale", {1, 3, 5, 7, 8, 10}},
    {L"Lydian Scale", {2, 4, 6, 7, 9, 11}},
    {L"Mixolydian Scale", {2, 4, 5, 7, 9, 10}},
    {L"Aeolian Scale", {2, 3, 5, 7, 8, 10}},
    {L"Locrian Scale", {1, 3, 5, 6, 8, 10}},
    {L"Harmonic Minor Scale", {2, 3, 5, 7, 8, 11}},
    {L"Locrian Natural 6 Scale", {1, 3, 5, 6, 9, 10}},
    {L"Augmented Major Scale", {2, 4, 5, 8, 9, 11}},
    {L"Dorian ♯11 Scale", {2, 3, 6, 7, 9, 10}},
    {L"Phrygian Dominant Scale", {1, 4, 5, 7, 8, 10}},
    {L"Lydian ♯2 Scale", {3, 4, 6, 7, 9, 11}},
    {L"Super Locrian ♭♭7 Scale", {1, 3, 4, 6, 8, 9}},
    {L"Jazz Minor Scale", {2, 3, 5, 7, 9, 11}},
    {L"Dorian ♭2 Scale", {1, 3, 5, 7, 9, 10}},
    {L"Lydian Augmented Scale", {2, 4, 6, 8, 9, 11}},
    {L"Lydian Dominant Scale", {2, 4, 6, 7, 9, 10}},
    {L"Aeolian Dominant Scale", {2, 4, 5, 7, 8, 10}},
    {L"Half-diminished Scale", {2, 3, 5, 6, 8, 10}},
    {L"Altered Scale", {1, 3, 4, 6, 8, 10}}
};
