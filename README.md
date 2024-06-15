# chordcat
Chord Naming App with MIDI Support
<img width="200" src="desktop/chordcat.png" align="right">

[Documentation](https://shriramters.github.io/chordcat/)

## WIP
Chord naming app with MIDI Support. Pre-Alpha.

All contributions welcome

<img src="https://github.com/shriramters/chordcat/assets/127589779/4ba040e4-29a8-4026-9d2c-e76a2ded030f">

## What works
- Chord Names will be displayed when: 
  - 1 or more MIDI devices are connected and you play a chord
  - you click the piano keys with a mouse
- Selecting preferred MIDI Device
- Piano Sounds
- Fullscreen / Resizing Window
- 3 Cool Fonts

## Building and Running
For more detailed information, please refer to the [Building From Source](https://shriramters.github.io/chordcat/docs/building-from-source/) section of the Chordcat Users' Manual.

Requires C++20

```bash
$ git clone https://github.com/shriramters/chordcat.git
$ cd chordcat
$ mkdir build && cd build
$ cmake .. # fix any missing dependencies on your OS such as libgtk2.0-dev
$ make -j$(nproc)
$ ./chordcat
```

## Built With
- [libremidi](https://github.com/jcelerier/libremidi)
- [SFML](https://www.sfml-dev.org/index.php)
- [fluidsynth](https://www.fluidsynth.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui-SFML](https://github.com/SFML/imgui-sfml)
- [Boost.PropertyTree](https://www.boost.org/doc/libs/1_84_0/doc/html/property_tree.html)
