// Copyright (C) 2024 ilobilo

#pragma once

#include <pieces.hpp>
#include <term.hpp>

#include <cstddef>

#include <random>
#include <array>

namespace tetris
{
    inline constexpr std::size_t hsquares = 10;
    inline constexpr std::size_t vsquares = 20;

    enum class movements : std::uint8_t
    {
        left, right,
        down, rotate
    };

    class board_t
    {
        private:
        terminal &term;

        std::array<std::array<std::size_t, hsquares>, vsquares> buffer;
        piece current_piece;
        piece ghost;
        bool falling;

        std::random_device rd;
        std::mt19937_64 mt;
        std::uniform_int_distribution<> dist;

        std::size_t history[4];
        std::size_t random();

        public:
        board_t(terminal &term) :
            term(term), buffer { }, current_piece { }, ghost { }, falling { false },
            rd { }, mt { rd() }, dist { 0, 34 }, history { 7, 0, 0, 0 } { }

        bool is_falling() const { return this->falling; }

        void add_piece();
        std::size_t clearlines();

        bool move(movements mv);

        bool check(const piece pc) const;
        void draw(std::string_view title) const;
    };

    class game
    {
        private:
        terminal term;
        board_t board;

        std::size_t level;
        std::size_t lines;

        public:
        bool game_over;

        game() : term { }, board { term }, level { 1 }, lines { 0 }, game_over { false } { }

        void start();

        std::pair<std::size_t, std::size_t> get_score() const
        {
            return { this->level, this->lines };
        }
    };
} // namespace tetris
