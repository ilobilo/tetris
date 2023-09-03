// Copyright (C) 2022  ilobilo

#pragma once

#include <frozen/map.h>
#include <random>
#include <memory>
#include <vector>
#include <atomic>
#include <array>

#include <term.hpp>

namespace tetris
{
    using postype = ssize_t;

    constexpr inline std::pair<postype, postype> board_size { 10, 20 };
    constexpr inline postype startx = board_size.first / 2;
    constexpr inline postype starty = 0;

    constexpr inline int used = 1;
    constexpr inline int unused = 0;

    enum class ids { none, I, O, Sr, Sl, Lr, Ll, T };
    enum class direction { right, left, down };

    struct board;
    struct piece
    {
        private:
        using storage_type = std::vector<std::vector<int>>;

        ids id;
        postype x;
        postype y;

        postype ymin;
        postype ymax;
        postype xmin;
        postype xmax;

        storage_type shape;
        board *brd;

        void getexts()
        {
            this->ymin = 0;
            this->ymax = 0;
            this->xmin = 0;
            this->xmax = 0;
            bool ym = true;
            bool xm = true;

            for (size_t yi = 0; const auto &y : shape)
            {
                for (size_t xi = 0; const auto &x : y)
                {
                    if (x == used)
                    {
                        if (ym == true)
                        {
                            ym = false;
                            this->ymin = yi;
                        }

                        if (xm == true)
                        {
                            xm = false;
                            this->xmin = xi;
                        }
                        else this->xmin = std::min(this->xmin, postype(xi));

                        this->xmax = std::max(this->xmax, postype(xi));
                        this->ymax = yi;
                    }
                    xi++;
                }
                yi++;
            }
        }

        public:
        piece(ids id, storage_type shape) :
            id(id), x(startx - (shape.size() - 2)), y(starty - 1), shape(std::move(shape)), brd(nullptr)
        { this->getexts(); }

        piece clone(postype nx, postype ny)
        {
            auto ret = *this;
            ret.x = nx;
            ret.y = ny;
            return ret;
        }

        void setboard(board *brd) { this->brd = brd; }

        std::pair<size_t, size_t> position() const
        {
            return { this->x, this->y };
        }
        ids type() const { return this->id; }
        postype size() const { return this->shape.size(); }

        postype getx() const { return this->x; }
        postype gety() const { return this->y; }

        postype getxmin() const { return this->xmin; }
        postype getxmax() const { return this->xmax; }
        postype getymin() const { return this->ymin; }
        postype getymax() const { return this->ymax; }

        bool getat(postype ax, postype ay) const
        {
            auto rx = ax - this->x;
            auto ry = ay - this->y;

            if (rx < 0 || rx > this->size() - 1)
                return false;

            if (ry < 0 || ry > this->size() - 1)
                return false;

            return this->shape[ry][rx];
        }
        const storage_type &data() const { return this->shape; }

        bool move(direction direct);
        void rotate();
    };

    struct board
    {
        std::array<std::array<int, board_size.first>, board_size.second> array;
        std::unique_ptr<piece> current_piece;

        board() : array { } { }

        bool add_piece(ids id);
        bool collides(piece pc);
        postype linefull();
        void linedel(postype ly);

        void copy();
        void draw();

        bool set_termsize();
    };

    struct game
    {
        term::window trm;
        board brd;

        std::atomic_bool keep_running;
        uint16_t score;

        std::random_device rd;
        std::mt19937_64 rnd;
        std::uniform_int_distribution<> uid;

        ids lastid;
        ids genid();

        game() : trm(), brd(), keep_running(true), score(0),  rd(), rnd(rd()), uid(1, 7), lastid(ids::none) { }

        void start();
        void redraw();
    };
} // namespace tetris