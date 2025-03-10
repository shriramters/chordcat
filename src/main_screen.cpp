// SPDX-License-Identifier: GPL-3.0-only
#include "main_screen.hpp"
#include "chord.hpp"
#include "config.h"
#include "gm_instrument_table.hpp"
#include "grand_staff.hpp"
#include "libremidi/api.hpp"
#include "libremidi/defaults.hpp"
#include "looper.hpp"
#include "metronome.hpp"
#include "midi_event.hpp"
#include "piano.hpp"
#include "preferences.hpp"
#include "sound_font_manager.hpp"
#include "utils.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>
#include <filesystem>
#include <fluidsynth.h>
#include <fluidsynth/synth.h>
#include <fluidsynth/version.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <libremidi/config.hpp>
#include <libremidi/libremidi.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

MainScreen::MainScreen(sf::RenderWindow& window) : AppState(window) {}

std::shared_ptr<AppState> MainScreen::Run() {

    // Setup Preferences
    Preferences preferences;
    if (!preferences.setup()) {
        return nullptr;
    }

    // ImGui
    if (!ImGui::SFML::Init(window))
        return nullptr;
    sf::Clock deltaClock;

    sf::Font font;
    if (!font.openFromFile(preferences.ui.font.path)) {
        std::cerr << "Error loading font" << std::endl;
    }

    auto title = sf::Text(font,"chordcat", 50u);
    auto chord_notes_text = sf::Text(font,"",  30u);
    std::vector<sf::Text> chord_name_list = {};
    // center the title
    title.setPosition({window.getSize().x / 2.f - title.getGlobalBounds().size.x / 2, 50});

    // Piano
    Piano piano(window, preferences.piano.pressed_note_colors);
    GrandStaff staves(window, font);

    SoundFontManager soundFontManager;
    auto soundfonts = soundFontManager.getAvailableSoundFonts();
    if (soundfonts.empty()) {
        std::cerr << "No soundfonts found. Did you delete the default soundfont?" << std::endl;
        return nullptr;
    }
    std::pair<std::string, std::filesystem::path> current_soundfont = soundfonts[0];
    int current_soundfont_id =
        fluid_synth_sfload(piano.getSynth(), current_soundfont.second.string().c_str(), 1);
    fluid_synth_set_gain(piano.getSynth(), preferences.piano.gain);

    int bpm = 120;
    Metronome metronome(piano.getSynth(), bpm, current_soundfont_id); // Initialize with 120 bpm
    Looper looper(piano, metronome);

    // Images
    sf::Texture logo;
    if (!logo.loadFromFile(std::string(APP_ASSETS_PATH) + "/images/chordcat.png")) {
        std::cerr << "Error loading logo" << std::endl;
    }

    std::function<void(const libremidi::message&)> onMidiMessage =
        [&](const libremidi::message& message) {
            if (message.size() == 3) {
                looper.recordEvent(message);
                piano.midiEvent(message);
            }
        };

    libremidi::midi_in midiin{{
        // Set our callback function.
        .on_message = onMidiMessage,
        .ignore_sysex = false,
        .ignore_timing = false,
        .ignore_sensing = false,
    }};

    libremidi::API current_api = midiin.get_current_api();

    std::string portName;
    auto ports = libremidi::observer{{}, observer_configuration_for(midiin.get_current_api())}
                     .get_input_ports();
    unsigned int nPorts = ports.size();
    if (nPorts >= 1) {
        midiin.open_port(ports[0]);
        portName = ports[0].display_name;
    }
    if (portName.empty())
        portName = "No MIDI Devices Found";

    auto portinfo_text = sf::Text(font, portName, 30u);
    portinfo_text.setPosition({window.getSize().x / 2.f - portinfo_text.getGlobalBounds().size.x / 2,
                              100});

    // TODO: Move these to a state object
    bool show_preferences = false;
    bool show_about = false;
    bool show_looper = false;
    bool show_instrument_table = false;
    Key key = Key::CMajor;
    staves.setKey(key);

    int program_channel = 0;
    std::array<int, 16> channel_programs;
    channel_programs.fill(0);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()){
                ImGui::SFML::ProcessEvent(window,*event);
                if (event->is<sf::Event::Closed>()){
                    window.close();
                }else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()){
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                        window.close();
                }else if (const auto* resized = event->getIf<sf::Event::Resized>()){
                    // resize my view
                    sf::FloatRect visibleArea({0, 0}, sf::Vector2f(resized->size));
                    window.setView(sf::View(visibleArea));
                    title.setPosition({window.getSize().x / 2.f - title.getGlobalBounds().size.x / 2, 50});
                    portinfo_text.setPosition({
                        window.getSize().x / 2.f - portinfo_text.getGlobalBounds().size.x / 2, 100});
                }
                piano.processEvent(*event);
        }

        std::vector<sf::String> pressed_notes =
            key_numbers_to_note_names(piano.getPressedNotes(), key);
        staves.updateNotes(piano.getPressedNotes());
        sf::String current_msg = "";
        for (auto& note : pressed_notes) {
            current_msg += note + " ";
        }
        if (!current_msg.isEmpty())
            current_msg.erase(current_msg.getSize() - 1, 1); // remove extra space
        auto chordset = name_that_chord(piano.getPressedNotes());
        chord_name_list = {};
        for (auto chord : chordset) {
            chord_name_list.push_back(sf::Text(font, chord.to_sf_string(key), 30u));
            chord_name_list.back().setPosition({window.getSize().x / 3.f,
                                               200.0f + 50.0f * chord_name_list.size()});
        }
        chord_notes_text = sf::Text(font, current_msg, 50u);
        chord_notes_text.setPosition({window.getSize().x / 3.f, 150});

        // Fluid Synth Stuff
        fluid_synth_set_gain(piano.getSynth(), preferences.piano.gain);

        looper.update();
        ImGui::SFML::Update(window, deltaClock.restart());

        if (show_preferences) {
            ImGui::Begin("Preferences", nullptr);
            ImGui::SetWindowSize({0, 0});
            if (ImGui::CollapsingHeader("User Interface")) {
                if (ImGui::TreeNode("Font Face")) {
                    if (ImGui::BeginCombo("##combo", font.getInfo().family.c_str())) {
                        for (int n = 0; n < available_fonts.size(); n++) {
                            bool is_selected =
                                (preferences.ui.font.name ==
                                 available_fonts[n]
                                     .first); // You can store your selection however you want,
                            // outside or inside your objects
                            if (ImGui::Selectable(available_fonts[n].first.c_str(), is_selected)) {
                                preferences.ui.font.name = available_fonts[n].first;
                                preferences.ui.font.path = available_fonts[n].second;
                                if (!font.openFromFile(preferences.ui.font.path)) {
                                    // load failed
                                    std::cerr << "Failed to load font: " << preferences.ui.font.path << std::endl;
                                }
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            if (ImGui::CollapsingHeader("Configuration")) {
                if (ImGui::TreeNode("MIDI Device")) {
                    if (ImGui::BeginCombo("##combo", portName.c_str())) {
                        for (int n = 0; n < ports.size(); n++) {
                            bool is_selected =
                                (portName ==
                                 ports[n].display_name); // You can store your selection however you
                            // want, outside or inside your objects
                            if (ImGui::Selectable(ports[n].display_name.c_str(), is_selected)) {
                                portName = ports[n].display_name;
                                midiin.close_port();
                                midiin.open_port(ports[n]);
                                portinfo_text.setString(portName);
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TreePop();
                    ImGui::Spacing();
                }

                if (ImGui::TreeNode("SoundFont")) {
                    if (ImGui::BeginCombo("##combo", current_soundfont.first.c_str())) {
                        for (const auto& soundfont : soundfonts) {
                            bool is_selected = (current_soundfont == soundfont);
                            if (ImGui::Selectable(soundfont.first.c_str(), is_selected)) {
                                current_soundfont = soundfont;
                                current_soundfont_id = fluid_synth_sfload(
                                    piano.getSynth(), soundfont.second.string().c_str(), 1);
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TreePop();
                    ImGui::Spacing();
                }

                if (ImGui::TreeNode("Backend")) {
                    if (ImGui::BeginCombo("##combo", to_string_view(current_api).data())) {
                        for (const auto& api : libremidi::available_apis()) {
                            bool is_selected = (current_api == api);
                            if (ImGui::Selectable(to_string_view(api).data(), is_selected)) {
                                current_api = api;
                                midiin.close_port();
                                midiin = libremidi::midi_in{
                                    {
                                        // Set our callback function.
                                        .on_message = onMidiMessage,
                                        .ignore_sysex = false,
                                        .ignore_timing = false,
                                        .ignore_sensing = false,
                                    },
                                    libremidi::midi_in_configuration_for(current_api)};
                                ports =
                                    libremidi::observer{{}, observer_configuration_for(current_api)}
                                        .get_input_ports();
                                nPorts = ports.size();
                                if (nPorts >= 1) {
                                    midiin.open_port(ports[0]);
                                    portName = ports[0].display_name;
                                }
                                if (portName.empty())
                                    portName = "No MIDI Devices Found";
                                portinfo_text.setString(portName);
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            if (ImGui::CollapsingHeader("Piano")) {
                if (ImGui::TreeNode("Program Change")) {
                    if (ImGui::InputInt("Channel", &program_channel, 1, 5)) {
                        program_channel = std::clamp(program_channel, 0, 15);
                    }
                    if (ImGui::InputInt("Instrument", &channel_programs[program_channel], 1, 5)) {
                        channel_programs[program_channel] =
                            std::clamp(channel_programs[program_channel], 0, 127);
                        fluid_synth_program_change(piano.getSynth(), program_channel,
                                                   channel_programs[program_channel]);
                    }
                    if (ImGui::Button("Show Instrument Table")) {
                        show_instrument_table = !show_instrument_table;
                    }
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Aspect Ratio")) {
                    ImGui::SliderFloat("KeyH/KeyW", &piano.key_aspect_ratio, 0.0f, 20.0f,
                                       "ratio = %.2f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Synth Gain")) {
                    ImGui::SliderFloat("Gain", &preferences.piano.gain, 0.0f, 10.0f, "%.2f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Pressed Note Color")) {
                    ImGui::ColorPicker4("Color", &preferences.piano.pressed_note_colors[0],
                                        ImGuiColorEditFlags_DefaultOptions_);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            ImGui::Text("Metronome Tempo");
            if (ImGui::InputInt("BPM", &bpm, 1, 5)) {
                bpm = std::clamp(bpm, 30, 300);
                metronome.setBpm(bpm);
            }
            int channel = piano.getChannel();
            ImGui::Text("PC Keyboard MIDI Channel");
            if (ImGui::InputInt("MIDI Channel", &channel, 1, 5)) {
                channel = std::clamp(channel, 0, 15);
                piano.setChannel(channel);
            }
            const char* keyNames[] = {
                "C Major / A Minor",   "G Major / E Minor",   "D Major / B Minor",
                "A Major / F# Minor",  "E Major / C# Minor",  "B Major / G# Minor",
                "F# Major / D# Minor", "C# Major / A# Minor", "F Major / D Minor",
                "Bb Major / G Minor",  "Eb Major / C Minor",  "Ab Major / F Minor",
                "Db Major / Bb Minor", "Gb Major / Eb Minor", "Cb Major / Ab Minor",
                "D# Major / B# Minor", "G# Major / E# Minor", "A# Major / Fx Minor"};

            int currentKey = static_cast<int>(key);
            ImGui::Text("Key");
            if (ImGui::Combo("Key", &currentKey, keyNames, IM_ARRAYSIZE(keyNames))) {
                key = static_cast<Key>(currentKey);
                staves.setKey(key);
            }
            ImGui::End();
        }

        if (show_about) {
#define STRINGIZE(s) #s
#define STRINGCEPTION(s) STRINGIZE(s)

            const std::vector<const char*> about_lines = {
                "Chordcat v" CHORDCAT_VERSION,
                "Chord Naming Application",
                "© 2024 Shriram Ravindranathan",
                "",
                // MACRO integers 2.X.X defined in SFML/Config.hpp
                "SFML Version: " STRINGCEPTION(SFML_VERSION_MAJOR) "." STRINGCEPTION(
                    SFML_VERSION_MINOR) "." STRINGCEPTION(SFML_VERSION_PATCH),
                "FluidSynth Version: " FLUIDSYNTH_VERSION,
                "Libremidi Version: " LIBREMIDI_VERSION,
            };

            ImGui::Begin("About", nullptr);
            ImGui::SetWindowSize({0, 0});
            
            sf::Sprite logoSprite(logo);
            logoSprite.setScale({0.2, 0.2});

            ImGui::SameLine((ImGui::GetWindowWidth() - logoSprite.getGlobalBounds().size.x) / 2);
            ImGui::Image(logoSprite);
            ImGui::NewLine();

            for (auto line : about_lines) {
                ImGui::SameLine((ImGui::GetWindowWidth() - ImGui::CalcTextSize(line).x) / 2);
                ImGui::Text("%s", line);
                ImGui::NewLine();
            }
            ImGui::End();
        }

        if (show_looper) {
            ImGui::Begin("Looper", nullptr);
            ImGui::SetWindowSize({0, 0});
            ImGui::Text("Set number of bars to record");
            int bars = looper.getBars();
            if (ImGui::InputInt("bars", &bars, 1, 5)) {
                bpm = std::clamp(bars, 1, 256);
                looper.setBars(bars);
            }
            switch (looper.getState()) {
            case LooperState::Idle: {
                ImGui::Text("%s", "There will be a count in of 4 beats");
                ImGui::NewLine();
                if (ImGui::Button("Start Recording"))
                    looper.startRecording();
                break;
            };
            case LooperState::CountingIn: {
                ImGui::Text("%s %lld", "Counting In, ", metronome.getBeat() - 4);
                ImGui::NewLine();
                if (ImGui::Button("Stop Recording"))
                    looper.stopRecording();
                break;
            }
            case LooperState::Recording: {
                ImGui::Text("%s %lld", "Recording, beat ", metronome.getBeat() - 4);
                ImGui::Text("%s %d", "Will stop recording at beat ", looper.getBars() * 4);
                ImGui::NewLine();
                if (ImGui::Button("Stop Recording"))
                    looper.stopRecording();
                break;
            }
            case LooperState::CountingOut: {
                ImGui::Text("%s %lld", "Stopping at end of bar, beat ", metronome.getBeat() - 4);
                ImGui::NewLine();
                if (ImGui::Button("Stop Recording"))
                    looper.stopRecording();
                break;
            }
            case LooperState::PlayingBack: {
                ImGui::Text("%s %lld", "Playing Back, beat ", metronome.getBeat() - 4);
                ImGui::NewLine();
                if (ImGui::Button("Stop Playback"))
                    looper.stopPlayback();
                if (ImGui::Button("Overdub"))
                    looper.startOverdub();
                break;
            }
            case LooperState::CountingInToOverdub: {
                ImGui::Text("%s %lld", "Begin overdub at the start of next loop. beat ",
                            metronome.getBeat() - 4);
                ImGui::NewLine();
                if (ImGui::Button("Cancel"))
                    looper.stopOverdub();
                break;
            }
            case LooperState::Overdubbing: {
                ImGui::Text("%s %lld", "Overdubbing, beat ", metronome.getBeat() - 4);
                ImGui::NewLine();
                if (ImGui::Button("Stop Overdub"))
                    looper.stopOverdub();
                if (ImGui::Button("Cancel Overdub"))
                    looper.cancelOverdub();
                break;
            }
            default: {
                ImGui::Text("%s", "Looper kaputt");
            }
            }
            ImGui::End();
        }
        if (show_instrument_table) {
            ImGui::Begin("Instrument Table", nullptr);
            ImGui::SetWindowSize({0, 0});
            showGMInstrumentTable();
            ImGui::End();
        }

        // The entire top bar with the buttons:(Clear All, Preferences, <-->, About)
        ImGui::Begin("MENUWINDOW", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::SetWindowPos({0, 0});
        ImGui::SetWindowSize({ImGui::GetIO().DisplaySize.x, ImGui::GetWindowHeight()});
        if (ImGui::Button("Clear All"))
            piano.clearAllKeys();
        ImGui::SameLine();
        if (ImGui::Button("Preferences"))
            show_preferences = !show_preferences;
        ImGui::SameLine();
        if (ImGui::Button("Metronome")) {
            metronome.toggle();
        }
        ImGui::SameLine();
        if (ImGui::Button("Looper")) {
            show_looper = !show_looper;
        }

        // Align to the right of screen (screenwidth - button size)
        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x -
                        ImGui::CalcTextSize("About").x - (ImGui::GetStyle().FramePadding.x * 2.f));
        if (ImGui::Button("About"))
            show_about = !show_about;
        ImGui::End();

        window.clear();
        window.draw(piano);
        window.draw(staves);
        for (auto chordname : chord_name_list) {
            window.draw(chordname);
        }
        window.draw(title);
        window.draw(portinfo_text);
        window.draw(chord_notes_text);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    midiin.close_port();
    if (!preferences.save())
        return nullptr;
    return nullptr;
}
