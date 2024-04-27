// Copyright (C) 2024  ilobilo

#include <tetris.hpp>
#include <iostream>
#include <cstdlib>

auto main() -> int
{
    std::size_t level = 0;
    std::size_t lines = 0;
    {
        tetris::game game;
        game.start();

        auto score = game.get_score();
        level = score.first;
        lines = score.second;
    }
    std::cout << "Game over. level: " << level << ", extra lines: " << lines << std::endl;

    return EXIT_SUCCESS;
}