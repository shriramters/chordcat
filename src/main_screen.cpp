#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <libremidi/libremidi.hpp>
#include "main_screen.hpp"
#include "piano.hpp"
#include "utils.hpp"

MainScreen::MainScreen(sf::RenderWindow& window) : AppState(window) {}

std::shared_ptr<AppState> MainScreen::Run() {

    sf::View view = window.getDefaultView();
    // load audio file from assets
    sf::SoundBuffer buffer;
    buffer.loadFromFile("assets/audio/title_bgm.mp3");
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sound.setLoop(true);

    // load background image from assets
    sf::Texture texture;
    if (!texture.loadFromFile("assets/images/main_screen.png"))
    {
        std::cerr << "Error loading title screen image" << std::endl;
    }

    // sf::Sprite sprite(texture);
    // sprite.setScale(window.getSize().x / sprite.getLocalBounds().width, window.getSize().y / sprite.getLocalBounds().height);

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/Open_Sans-2/static/OpenSans-Light.ttf"))
    {
        std::cerr << "Error loading font" << std::endl;
    }

    auto title = sf::Text{ "chordcat", font, 50u };
    auto chord_notes_text = sf::Text("", font, 30u);
    std::vector<sf::Text> chord_name_list = {};
    // center the title
    title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 50);

    // Piano
    Piano piano;

    libremidi::midi_in midiin{ {
            // Set our callback function.
            .on_message = [&](const libremidi::message& message) {
                if (message.size() == 3 && (int)message[0] == 144) {
                    if ((int)message[2] == 0) {
                        piano.setKeyPressed((int)message[1],false);
                    }
                    else {
                        piano.setKeyPressed((int)message[1],true);
                    }
                }
                std::vector<std::string> pressed_notes = key_numbers_to_note_names(piano.getPressedNotes());
                std::string current_msg = "";
                for (auto& note : pressed_notes) {
                    current_msg += note + " ";
                }
                if (!current_msg.empty()) current_msg.pop_back(); // remove extra space
                auto chordset = name_that_chord(piano.getPressedNotes());
                chord_name_list = {};
                for (auto chord : chordset) {
                    chord_name_list.push_back(sf::Text(chord.to_string(),font,30u));
                    chord_name_list.back().setPosition(window.getSize().x / 3, 200 + 50 * chord_name_list.size());
                }
                chord_notes_text = sf::Text(current_msg, font, 50u);
                chord_notes_text.setPosition(window.getSize().x / 2 - chord_notes_text.getGlobalBounds().width / 2, 150);
             },
        .ignore_sysex = false,
        .ignore_timing = false,
        .ignore_sensing = false,
    } };

    std::string portName;
    auto ports = libremidi::observer{ {}, observer_configuration_for(midiin.get_current_api()) }
    .get_input_ports();
    unsigned int nPorts = ports.size();
    if (nPorts == 1)
    {
        midiin.open_port(ports[0]);
        portName = ports[0].display_name;
    }
    if (portName.empty())
        portName = "couldn't choose device";

    auto portinfo_text = sf::Text(portName, font, 30u);
    portinfo_text.setPosition(window.getSize().x / 2 - portinfo_text.getGlobalBounds().width / 2, 100);

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
                portinfo_text.setPosition(window.getSize().x / 2 - portinfo_text.getGlobalBounds().width / 2, 100);
            }
        }
        window.clear();
        // window.draw(sprite);
        window.draw(piano);
        for (auto chordname : chord_name_list) {
            window.draw(chordname);
        }
        window.draw(title);
        window.draw(portinfo_text);
        window.draw(chord_notes_text);
        window.display();
    }
    sound.stop();
    return nullptr;
}