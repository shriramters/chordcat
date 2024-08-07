// SPDX-License-Identifier: GPL-3.0-only
#include "main_screen.hpp"
#include "config.h"
#include "libremidi/config.hpp"
#include "midi_audio_stream.hpp"
#include "piano.hpp"
#include "preferences.hpp"
#include "utils.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>
#include <SFML/Config.hpp>
#include <boost/version.hpp>
#include <fluidsynth.h>
#include <fluidsynth/version.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <libremidi/libremidi.hpp>
#include <string>
#include <vector>

MainScreen::MainScreen(sf::RenderWindow& window) : AppState(window) {}

std::shared_ptr<AppState> MainScreen::Run() {

    // Setup Preferences
    Preferences preferences;
    std::optional<std::string> appdata_path = get_appdata_path();
    if (appdata_path.has_value()) {
        try {
            preferences.load(*appdata_path + "/settings.xml");
        } catch (boost::wrapexcept<boost::property_tree::xml_parser::xml_parser_error> e) {
            std::error_code err;
            if (!CreateDirectoryRecursive(*appdata_path, err)) {
                std::cout << "CreateDirectoryRecursive FAILED, err: " << err.message() << std::endl;
                return nullptr;
            }
            preferences.save(*appdata_path + "/settings.xml");
        } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>) {
            // if a node is missing / settings file was tampered improperly
            // reset the settings file with sensible defaults
            preferences.save(*appdata_path + "/settings.xml");
        }
    } else {
        return nullptr;
    }

    // ImGui
    if (!ImGui::SFML::Init(window))
        return nullptr;
    sf::Clock deltaClock;

    sf::Font font;
    if (!font.loadFromFile(preferences.ui.font.path)) {
        std::cerr << "Error loading font" << std::endl;
    }

    auto title = sf::Text{L"c♯ordcat", font, 50u};
    auto chord_notes_text = sf::Text("", font, 30u);
    std::vector<sf::Text> chord_name_list = {};
    // center the title
    title.setPosition(window.getSize().x / 2.f - title.getGlobalBounds().width / 2, 50);

    // Piano
    std::string sound_font_path = std::string(APP_ASSETS_PATH) + "/soundfonts/TimGM6mb.sf2";
    Piano piano(window, preferences.piano.pressed_note_colors);
    fluid_synth_sfload(piano.getSynth(), sound_font_path.c_str(), 1);
    fluid_synth_set_gain(piano.getSynth(), preferences.piano.gain);

    // Images
    sf::Texture logo;
    if (!logo.loadFromFile(std::string(APP_ASSETS_PATH) + "/images/chordcat.png")) {
        std::cerr << "Error loading logo" << std::endl;
    }

    libremidi::midi_in midiin{{
        // Set our callback function.
        .on_message =
            [&](const libremidi::message& message) {
                // 0th byte: status byte containing message type and channel. we only care for the first nibble, see if it is noteOn or noteOff while ignoring the channel
                // 1st byte: note number (for noteOn and noteOff)
                // 2nd byte: velocity (for noteOn and noteOff)
                if (message.size() == 3) {
                    switch (message[0] >> 4) {
                        case 9:
                            if ((int)message[2] > 0) {
                                piano.keyOn((int)message[1], (int)message[2]);
                                break;
                            }
                        case 8:
                            piano.keyOff((int)message[1]);
                            break;
                        default:
                            break;
                    }
                }
            },
        .ignore_sysex = false,
        .ignore_timing = false,
        .ignore_sensing = false,
    }};

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

    auto portinfo_text = sf::Text(portName, font, 30u);
    portinfo_text.setPosition(window.getSize().x / 2.f - portinfo_text.getGlobalBounds().width / 2,
                              100);

    // TODO: Move these to a state object
    bool show_preferences = false;
    bool show_about = false;

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
                title.setPosition(window.getSize().x / 2.f - title.getGlobalBounds().width / 2, 50);
                portinfo_text.setPosition(
                    window.getSize().x / 2.f - portinfo_text.getGlobalBounds().width / 2, 100);
            }
            ImGui::SFML::ProcessEvent(event);
            piano.processEvent(event);
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
            chord_name_list.back().setPosition(window.getSize().x / 3.f,
                                               200 + 50 * chord_name_list.size());
        }
        chord_notes_text = sf::Text(current_msg, font, 50u);
        chord_notes_text.setPosition(window.getSize().x / 3.f, 150);

        // Fluid Synth Stuff
        fluid_synth_set_gain(piano.getSynth(), preferences.piano.gain);

        ImGui::SFML::Update(window, deltaClock.restart());

        if (show_preferences) {
            ImGui::Begin("Preferences", nullptr);
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
                                font.loadFromFile(preferences.ui.font.path);
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
            }
            if (ImGui::CollapsingHeader("Piano")) {
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
            ImGui::End();
        }

        if (show_about) {
            #define STRINGIZE(s) #s
            #define STRINGCEPTION(s) STRINGIZE(s)

            const std::vector<const char *> about_lines = {
                "Chordcat v" CHORDCAT_VERSION,
                "Chord Naming Application",
                "© 2024 Shriram Ravindranathan",
                "",
                // MACRO integers 2.X.X defined in SFML/Config.hpp 
                "SFML Version: " STRINGCEPTION(SFML_VERSION_MAJOR)
                "." STRINGCEPTION(SFML_VERSION_MINOR)
                "." STRINGCEPTION(SFML_VERSION_PATCH),
                "FluidSynth Version: " FLUIDSYNTH_VERSION,
                "Libremidi Version: " LIBREMIDI_VERSION,
                "Boost Version: " BOOST_LIB_VERSION
            };
            
            ImGui::Begin("About", nullptr);
            ImGui::SetWindowSize({0,0});

            sf::Sprite logoSprite;
            logoSprite.setTexture(logo);
            logoSprite.setScale(0.2, 0.2);
            ImGui::SameLine((ImGui::GetWindowWidth() - logoSprite.getGlobalBounds().width) / 2);
            ImGui::Image(logoSprite);
            ImGui::NewLine();
            
            for(auto line : about_lines) {
                ImGui::SameLine((ImGui::GetWindowWidth() - ImGui::CalcTextSize(line).x) / 2);
                ImGui::Text("%s", line);
                ImGui::NewLine();
            }
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
        // Align to the right of screen (screenwidth - button size)
        ImGui::SameLine(ImGui::GetWindowWidth()
                        - ImGui::GetStyle().WindowPadding.x
                        - ImGui::CalcTextSize("About").x
                        - (ImGui::GetStyle().FramePadding.x * 2.f));
        if (ImGui::Button("About"))
            show_about = !show_about;
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
    midiin.close_port();
    preferences.save(*appdata_path + "/settings.xml");
    return nullptr;
}
