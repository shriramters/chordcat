# Chordcat

<img width="150px" src="desktop/chordcat.png" align="right">

Chord Naming App with MIDI Support

Chordcat is a desktop app that helps you identify and name chords based on inputted notes. 

- [Documentation](https://shriramters.github.io/chordcat/)
- Download the latest release [here](https://github.com/shriramters/chordcat/releases/tag/v0.2.1)

## Features

- Displays chord names when:
  - MIDI devices are connected and you play a chord
  - Piano keys are clicked using the mouse
  - The corresponding PC Keyboard keys are pressed 
- Select MIDI device
- Inbuilt piano sounds
- Supports fullscreen mode and resizable window mode.
- Includes 3 fun fonts (DejaVu Sans, Petaluma and FirstTimeWriting).

## Screenshots

<img src="https://github.com/user-attachments/assets/fce29f4a-9113-47ff-9a61-bc16081508e0" height="400px" width="608px">
<img src="https://github.com/shriramters/chordcat/assets/127589779/697dc8d9-b8d6-412e-be2d-fea71f324311" height="400px"  width="608px">

## Installing From Binaries

For installation instructions using pre-built binaries, refer to the [Chordcat User's Manual](https://shriramters.github.io/chordcat/docs/installing-from-binaries/).

## Building and Running From Source

To build and run chordcat from source, follow these steps:

1. Clone the repository:
   ```bash
   $ git clone https://github.com/shriramters/chordcat.git
   $ cd chordcat
   ```

2. Create a build directory and navigate into it:
   ```bash
   $ mkdir build && cd build
   ```

3. Configure the build using CMake. Ensure all dependencies are installed on your system.
   (refer to [Building From Source](https://shriramters.github.io/chordcat/docs/building-from-source/) section of the chordcat user's manual):
   ```bash
   $ cmake ..
   ```

4. Build the application using `make`:
   ```bash
   $ make -j$(nproc)
   ```

5. Run Chordcat:
   ```bash
   $ ./chordcat
   ```

## Built With (Thanks to)
- [libremidi](https://github.com/jcelerier/libremidi)
- [SFML](https://www.sfml-dev.org/index.php)
- [FluidSynth](https://www.fluidsynth.org/)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui-SFML](https://github.com/SFML/imgui-sfml)
- [Boost.PropertyTree](https://www.boost.org/doc/libs/release/doc/html/property_tree.html)
