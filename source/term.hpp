// Copyright (C) 2024 ilobilo

#pragma once

#include <string_view>

#include <cstddef>
#include <cassert>
#include <locale>

#include <ncurses.h>

namespace tetris
{
    namespace chars
    {
        inline constexpr auto full =  "██";
        inline constexpr auto falling = "▒▒";

        inline constexpr std::size_t nch = 2;

        namespace border
        {
            inline constexpr auto vertical = "┃";
            inline constexpr auto horizontal = "━";

            namespace corners
            {
                inline constexpr auto topleft = "┏";
                inline constexpr auto topright = "┓";
                inline constexpr auto botleft = "┗";
                inline constexpr auto botright = "┛";
            } // namespace corners
        } // namespace border
    } // namespace chars

    struct terminal
    {
        bool colours;

        terminal()
        {
            std::locale::global(std::locale(""));

            auto scr = initscr();
            this->colours = has_colors();

            if (this->colours)
            {
                start_color();
                use_default_colors();

                init_pair(1, COLOR_CYAN, -1);
                init_pair(2, COLOR_BLUE, -1);
                init_pair(3, COLOR_WHITE /* orange how? */, -1);
                init_pair(4, COLOR_YELLOW, -1);
                init_pair(5, COLOR_GREEN, -1);
                init_pair(6, COLOR_RED, -1);
                init_pair(7, COLOR_MAGENTA, -1);
                init_pair(8, COLOR_WHITE, -1);
            }

            keypad(scr, TRUE);
            nodelay(scr, TRUE);

            cbreak();
            noecho();
            curs_set(0);

            clear();
            refresh();
        }

        ~terminal()
        {
            endwin();
        }

        std::size_t width() { return COLS; }
        std::size_t height() { return LINES; }

        void refresh() { ::refresh(); }
        void clear() { ::erase(); }

        auto getkey()
        {
            return getch();
        }

        void printat(std::size_t x, std::size_t y, std::string_view str, std::size_t colour)
        {
            if (this->colours)
                attron(COLOR_PAIR(colour));
            mvprintw(y, x, "%.*s", static_cast<int>(str.length()), str.data());
            if (this->colours)
                attroff(COLOR_PAIR(colour));
        }
    };
} // namespace tetris
