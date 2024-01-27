#pragma once
#include <vector>
#include <string>
#include <set>

//TODO: constexpr in future
const std::vector<std::pair<const std::string, std::set<unsigned short> > > chord_db = {
    {"sus2",{2,7}},
    {"minor",{3,7}},
    {"dim",{3,6}},
    {"aug",{3,8}},
    {"major",{4,7}},
    {"sus4",{5,7}},
    {"maj7",{4,7,11}},
    {"min7",{3,7,10}},
    {"6",{4,7,9}},
    {"7",{4,7,10}},
    {"minMaj7",{3,7,11}},
    {"dim7",{3,6,9}},
    {" Half-diminished",{3,6,10}},
    {" Major Scale",{2,4,5,7,9,11}},
    {" Dorian Scale",{2,3,5,7,9,10}},
    {" Phrygian Scale",{1,3,5,7,8,10}},
    {" Lydian Scale",{2,4,6,7,9,11}},
    {" Mixolydian Scale",{2,4,5,7,9,10}},
    {" Aeolian Scale",{2,3,5,7,8,10}},
    {" Locrian Scale",{1,3,5,6,8,10}},
    {" Harmonic Minor Scale",{2,3,5,7,8,11}},
    {" Locrian Natural 6 Scale",{1,3,5,6,9,10}},
    {" Augmented Major Scale",{2,4,5,8,9,11}},
    {" Dorian #11 Scale",{2,3,6,7,9,10}},
    {" Phrygian Dominant Scale",{1,4,5,7,8,10}},
    {" Lydian #2 Scale",{3,4,6,7,9,11}},
    {" Super Locrian bb7 Scale",{1,3,4,6,8,9}},
    {" Jazz Minor Scale",{2,3,5,7,9,11}},
    {" Dorian b2 Scale",{1,3,5,7,9,10}},
    {" Lydian Augmented Scale",{2,4,6,8,9,11}},
    {" Lydian Dominant Scale",{2,4,6,7,9,10}},
    {" Aeolian Dominant Scale",{2,4,5,7,8,10}},
    {" Half-diminished Scale",{2,3,5,6,8,10}},
    {" Altered Scale",{1,3,4,6,8,10}},
};