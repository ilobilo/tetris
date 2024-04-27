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
                inline constexpr auto botleft = "┗";
                inline constexpr auto botright = "┛";
            } // namespace corners
        } // namespace border
    } // namespace chars

    struct terminal
    {
        terminal()
        {
            std::locale::global(std::locale(""));

            auto scr = initscr();

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
        void clear() { ::clear(); }

        auto getkey()
        {
            return getch();
        }

        void printat(std::size_t x, std::size_t y, std::string_view str)
        {
            mvprintw(y, x, "%.*s", static_cast<int>(str.length()), str.data());
        }
    };
} // namespace tetris