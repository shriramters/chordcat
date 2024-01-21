#include <SFML/Graphics.hpp>
#include "main_screen.hpp"

int main()
{
    auto window = sf::RenderWindow{ { 1000u, 800u }, "chordcat" };
    window.setFramerateLimit(144);

    std::shared_ptr<AppState> state = std::static_pointer_cast<AppState>(std::make_shared<MainScreen>(window));
    while (window.isOpen() && state != nullptr)
    {
        state = state->Run();
    }
}