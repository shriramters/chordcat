#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include "app_state.hpp"

class MainScreen : public AppState {
public:
    MainScreen(sf::RenderWindow& window);
    ~MainScreen() = default;

    std::shared_ptr<AppState> Run() override;
};
