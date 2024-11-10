# Chordcat

<img width="150px" src="desktop/chordcat.png" align="right">

Chord Naming App with MIDI Support

Chordcat is a desktop app that helps you identify and name chords based on inputted notes. 

- [Documentation](https://shriramters.github.io/chordcat/)
- Download the latest release [here](https://github.com/shriramters/chordcat/releases/tag/v0.3.0)
  
<a href='https://flathub.org/apps/dev.ters.Chordcat'>
  <img width='240' alt='Get it on Flathub' src='https://flathub.org/api/badge?locale=en'/>
</a>

## Features

- Displays chord names when:
  - MIDI devices are connected and you play a chord
  - Piano keys are clicked using the mouse
  - The corresponding PC Keyboard keys are pressed
- MIDI device support
- Metronome
- Looper
- 128 Inbuilt instruments (General MIDI)
- Custom `.sf2` soundfonts support
- 4 cool fonts

## Screenshots

<img src="https://shriramters.github.io/chordcat/chordcat-0.3.0-kde-with-midi.png" width="600px">
<img src="https://shriramters.github.io/chordcat/chordcat-0.3.0-kde-2.png" width="600px">

## Keybindings
<img src="https://github.com/user-attachments/assets/3c664e05-fee9-46aa-9c26-79f4e57f4e91" width="600px">

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
- [nlohmann-json](https://github.com/nlohmann/json)
