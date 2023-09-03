// Copyright (C) 2022  ilobilo

#pragma once

#include <termios.h>

namespace term
{
    enum class action
    {
        none,
        exit,
        move_right,
        move_left,
        speedup,
        rotate
    };
    struct window
    {
        termios og_tios;

        window();
        ~window();

        void reset();
        action getkey();
    };
} // namespace term