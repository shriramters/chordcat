#pragma once
#include <vector>
#include <string>
#include <set>

//TODO: constexpr in future
const std::vector<std::pair<const std::string, std::set<unsigned short> > > chord_db = {
    {"sus2",{2,7}},
    {"minor",{3,7}},
    {"dim",{3,6}},
    {"major",{4,7}},
    {"sus4",{5,7}},
    {"maj7",{4,7,11}},
    {"min7",{3,7,10}},
    {"7",{4,7,10}},
    {"minMaj7",{3,7,11}},
    {" half diminished",{3,6,10}},
    {" major scale",{2,4,5,7,9,11}},
    {" dorian scale",{2,3,5,7,9,10}},
    {" phrygian scale",{1,3,5,7,8,10}},
    {" lydian scale",{2,4,6,7,9,11}},
    {" mixolydian scale",{2,4,5,7,9,10}},
    {" aeolian scale",{2,3,5,7,8,10}},
    {" locrian scale",{1,3,5,6,8,10}},
};