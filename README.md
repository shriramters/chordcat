# chordcat
Chord Naming App with MIDI Support
<img width="200" src="https://github.com/shriramters/chordcat/assets/127589779/08db3d3e-8201-48b0-afa3-d79c098f3d83" align="right">


## WIP
Chord naming app with MIDI Support. Pre-Alpha.

All contributions welcome

<img src="https://github.com/shriramters/chordcat/assets/127589779/b4feda31-2d8f-49ad-a0f0-88fdb09451c2">

## What works
- Chord Names will be displayed when: 
  - 1 or more MIDI devices are connected and you play a chord
  - you click the piano keys with a mouse
- Selecting preferred MIDI Device
- Piano Sounds
- Fullscreen / Resizing Window

## Building and Running

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
