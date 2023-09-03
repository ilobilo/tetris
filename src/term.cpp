// Copyright (C) 2022  ilobilo

#include <fmt/core.h>
#include <term.hpp>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace term
{
    window::window()
    {
        termios tios;
        tcgetattr(STDIN_FILENO, &tios);
        this->og_tios = tios;

        tios.c_lflag &= ~(ECHO | ICANON);
        tios.c_cc[VMIN] = 0;
        tios.c_cc[VTIME] = 0;

        tcsetattr(STDIN_FILENO, TCSANOW, &tios);

        fmt::print("\033[2J\033[H");
    }

    window::~window()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &this->og_tios);
    }

    void window::reset()
    {
        fmt::print("\033[H");
    }

    action window::getkey()
    {
        int c = 0;

        again:
        read(STDIN_FILENO, &c, 1);
        switch (c)
        {
            case 'A':
            case 'w':
                return action::rotate;
            case 'B':
            case 's':
                return action::speedup;
            case 'C':
            case 'd':
                return action::move_right;
            case 'D':
            case 'a':
                return action::move_left;
            case 'q':
            case 'x':
                return action::exit;
            case '\033':
            case '[':
                goto again;
            default:
                break;
        }
        return action::none;
    }
} // namespace term