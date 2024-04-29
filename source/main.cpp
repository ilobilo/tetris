// Copyright (C) 2024  ilobilo

#include <tetris.hpp>
#include <iostream>
#include <cstdlib>

#include <signal.h>

tetris::game game;
auto main() -> int
{
    signal(SIGINT, [](int) { game.game_over = true; });
    game.start();

    auto [level, lines] = game.get_score();
    std::cout << "Game over. level: " << level << ", extra lines: " << lines << std::endl;

    return EXIT_SUCCESS;
}