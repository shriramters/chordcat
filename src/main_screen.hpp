// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include "app_state.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class MainScreen : public AppState {
public:
    MainScreen(sf::RenderWindow &window);
    ~MainScreen() = default;

    std::shared_ptr<AppState> Run() override;
};
