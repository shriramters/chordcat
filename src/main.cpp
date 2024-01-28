// SPDX-License-Identifier: GPL-3.0-only
#include <SFML/Graphics.hpp>
#include "main_screen.hpp"
#include "appicon.c"

int main()
{
    auto window = sf::RenderWindow{ { 1000u, 800u }, "chordcat" };
    window.setIcon(chordcat_icon.width, chordcat_icon.height, chordcat_icon.pixel_data);
    window.setFramerateLimit(144);

    std::shared_ptr<AppState> state = std::static_pointer_cast<AppState>(std::make_shared<MainScreen>(window));
    while (window.isOpen() && state != nullptr)
    {
        state = state->Run();
    }
}