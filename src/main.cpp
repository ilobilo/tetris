// Copyright (C) 2022  ilobilo

#include <tetris.hpp>
#include <signal.h>

static tetris::game game;
auto main() -> int
{
    game.start();
    signal(SIGINT, [](int) { game.keep_running = false; });
    return EXIT_SUCCESS;
}

