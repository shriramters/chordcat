// SPDX-License-Identifier: GPL-3.0-only
#include "main_screen.hpp"
#include "midi_audio_stream.hpp"
#include "piano.hpp"
#include "utils.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <fluidsynth.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <libremidi/libremidi.hpp>
#include <iostream>
#include <string>
#include <vector>

MainScreen::MainScreen(sf::RenderWindow& window) : AppState(window) {}

std::shared_ptr<AppState> MainScreen::Run() {

    // Audio
    MidiAudioStream mas;
    fluid_synth_t* synth = mas.getSynth();
    fluid_synth_sfload(synth, "assets/soundfonts/TimGM6mb.sf2", 1);
    // TODO: all these need imgui widgets
    fluid_synth_set_gain(synth, 2.0);

    // ImGui
    ImGui::SFML::Init(window);
    sf::Clock deltaClock;

    sf::View view = window.getDefaultView();

    //TODO: std::filesystem Path
    std::vector <std::pair<std::string, std::string> > fonts = {
        {"DejaVu Sans", "assets/fonts/DejaVuSans/ttf/DejaVuSans.ttf"},
        {"FirstTimeWriting!", "assets/fonts/FirstTimeWriting/FirstTimeWriting!.ttf"},
        {"Petaluma", "assets/fonts/Petaluma/otf/PetalumaScript.otf"}
    };
    std::pair<std::string, std::string> selected_font = fonts[0];

    //TODO: Remember choice from config.ini
    sf::Font font;
    if (!font.loadFromFile(selected_font.second)) {
        std::cerr << "Error loading font" << std::endl;
    }

    auto title = sf::Text{ L"c♯ordcat", font, 50u };
    auto chord_notes_text = sf::Text("", font, 30u);
    std::vector<sf::Text> chord_name_list = {};
    // center the title
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    // Piano
    Piano piano;
    float piano_note_color[4] = { 0.f,0.f,0.f,0.f };
    mas.play();

    libremidi::midi_in midiin{
        {
            // Set our callback function.
            .on_message =
                [&](const libremidi::message& message) {
                    if (message.size() == 3 && (int)message[0] == 144) {
                        if ((int)message[2] == 0) {
                            piano.setKeyPressed((int)message[1], false);
                            fluid_synth_noteoff(synth, 0, (int)message[1]);
                        }
                        else {
                        piano.setKeyPressed((int)message[1], true);
                        fluid_synth_noteon(synth, 0, (int)message[1], (int)message[2]);
                        }
                    }
                },
            .ignore_sysex = false,
            .ignore_timing = false,
            .ignore_sensing = false,
        }
    };

    std::string portName;
    auto ports = libremidi::observer{ {}, observer_configuration_for(midiin.get_current_api()) }
    .get_input_ports();
    unsigned int nPorts = ports.size();
    if (nPorts >= 1) {
        midiin.open_port(ports[0]);
        portName = ports[0].display_name;
    }
    if (portName.empty())
        portName = "No MIDI Devices Found";

    auto portinfo_text = sf::Text(portName, font, 30u);
    portinfo_text.setPosition(window.getSize().x / 2 - portinfo_text.getGlobalBounds().width / 2,
        100);

    bool show_preferences = false;

    while (window.isOpen()) {
        auto event = sf::Event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            if (event.type == sf::Event::Resized) {
                // resize my view
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);
                portinfo_text.setPosition(
                    window.getSize().x / 2 - portinfo_text.getGlobalBounds().width / 2, 100);
            }
            ImGui::SFML::ProcessEvent(event);
            piano.mouseEvent(event, window, synth);
        }
        std::vector<sf::String> pressed_notes = key_numbers_to_note_names(piano.getPressedNotes());
        sf::String current_msg = "";
        for (auto& note : pressed_notes) {
            current_msg += note + " ";
        }
        if (!current_msg.isEmpty())
            current_msg.erase(current_msg.getSize() - 1, 1); // remove extra space
        auto chordset = name_that_chord(piano.getPressedNotes());
        chord_name_list = {};
        for (auto chord : chordset) {
            chord_name_list.push_back(sf::Text(chord.to_sf_string(), font, 30u));
            chord_name_list.back().setPosition(window.getSize().x / 3,
                200 + 50 * chord_name_list.size());
        }
        chord_notes_text = sf::Text(current_msg, font, 50u);
        chord_notes_text.setPosition(window.getSize().x / 3, 150);

        ImGui::SFML::Update(window, deltaClock.restart());

        if (show_preferences) {
            ImGui::Begin("Preferences", nullptr);
            if (ImGui::CollapsingHeader("User Interface"))
            {
                if (ImGui::TreeNode("Font Face")) {
                    if (ImGui::BeginCombo("##combo", font.getInfo().family.c_str()))
                    {
                        for (int n = 0; n < fonts.size(); n++)
                        {
                            bool is_selected = (selected_font.first == fonts[n].first); // You can store your selection however you want, outside or inside your objects
                            if (ImGui::Selectable(fonts[n].first.c_str(), is_selected)) {
                                selected_font = fonts[n];
                                font.loadFromFile(selected_font.second);
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
            if (ImGui::CollapsingHeader("Configuration"))
            {
                if (ImGui::TreeNode("MIDI Device")) {
                    if (ImGui::BeginCombo("##combo", portName.c_str()))
                    {
                        for (int n = 0; n < ports.size(); n++)
                        {
                            bool is_selected = (portName == ports[n].display_name); // You can store your selection however you want, outside or inside your objects
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
            }
            if (ImGui::CollapsingHeader("Piano")) {
                if (ImGui::TreeNode("Aspect Ratio")) {
                    ImGui::SliderFloat("KeyH/KeyW", &piano.key_aspect_ratio, 0.0f, 20.0f, "ratio = %.2f");
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
                if (ImGui::TreeNode("Pressed Note Color")) {
                    ImGui::ColorPicker4("Color", &piano.note_colors[0], ImGuiColorEditFlags_DefaultOptions_);
                    ImGui::TreePop();
                    ImGui::Spacing();
                }
            }
            ImGui::End();
        }

        ImGui::Begin("MENUWINDOW", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        ImGui::SetWindowPos({ 0,0 });
        if (ImGui::Button("Clear All"))
            piano.clearAllKeys();
        ImGui::SameLine();
        if (ImGui::Button("Preferences"))
            show_preferences = !show_preferences;
        ImGui::End();

        window.clear();
        window.draw(piano);
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
    return nullptr;
}
