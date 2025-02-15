// SPDX-License-Identifier: GPL-3.0-only
#include "appicon.hpp"
#include "main_screen.hpp"
#include <SFML/Graphics.hpp>

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({1114u, 642u}), "chordcat");
    window.setIcon(sf::Vector2u(chordcat_icon.width, chordcat_icon.height), chordcat_icon.pixel_data);
    window.setFramerateLimit(144);

    std::shared_ptr<AppState> state =
        std::static_pointer_cast<AppState>(std::make_shared<MainScreen>(window));
    while (window.isOpen() && state != nullptr) {
        state = state->Run();
    }
}
