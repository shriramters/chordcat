#pragma once
#include <memory>
#include <SFML/Graphics.hpp>

// App State class which will be inherited by all game states
// such as the main menu, the game, the pause menu, etc.

class AppState {
protected:
    sf::RenderWindow& window;

public:
    AppState(sf::RenderWindow& window) : window(window) {};
    virtual ~AppState() = default;

    virtual std::shared_ptr<AppState> Run() = 0;
};