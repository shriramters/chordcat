// SPDX-License-Identifier: GPL-3.0-only
#pragma once

enum class Key {
    CMajor,
    GMajor,
    DMajor,
    AMajor,
    EMajor,
    BMajor,
    FSharpMajor, // enharmonic with G♭ major
    CSharpMajor, // enharmonic with D♭ major
    FMajor,
    BbMajor, // enharmonic with A♯ major
    EbMajor, // enharmonic with D♯ major
    AbMajor, // enharmonic with G♯ major

    // Enharmonic keys:
    DbMajor,     // enharmonic with C♯ major
    GbMajor,     // enharmonic with F♯ major
    CbMajor,     // enharmonic with B major
    DSharpMajor, // enharmonic with E♭ major
    GSharpMajor, // enharmonic with A♭ major
    ASharpMajor  // enharmonic with B♭ major
};

inline bool is_sharp_key(Key key) {
    switch (key) {
    case Key::CMajor:
    case Key::FMajor:
    case Key::BbMajor:
    case Key::EbMajor:
    case Key::AbMajor:
    case Key::DbMajor:
    case Key::GbMajor:
    case Key::CbMajor:
        return false;
    default:
        return true;
    }
}

inline bool is_flat_key(Key key) { return !is_sharp_key(key); }
