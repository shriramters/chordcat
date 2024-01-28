# chordcat
Chord Naming App with MIDI Support

## WIP
Only works when there is just one MIDI device plugged in to the computer. Will fix in future updates.

All contributions welcome

<img src="https://github.com/shriramters/chordcat/assets/127589779/f4c9e039-ac61-482e-9583-ee3e2f4f4452" width="600">

## What works
- Chord Names will be displayed when: 
  - 1 midi keyboard is connected and you play a chord
  - you click the piano keys with a mouse
- Piano Sounds
- Resizing the window

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