// Copyright (C) 2022  ilobilo

#include <fmt/ranges.h>
#include <tetris.hpp>
#include <chrono>

#ifdef TETRIS_GREEN_TEXT
#  include <fmt/color.h>
#  define _TETRIS_COLOUR fmt::fg(fmt::terminal_color::green),
#else
#  define _TETRIS_COLOUR
#endif

namespace tetris
{
    void piece::getexts()
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

    bool piece::move(direction direct)
    {
        switch (direct)
        {
            case direction::left:
                if (this->x + this->xmin > 0 && this->brd->collides(this->clone(this->x - 1, this->y)) == false)
                {
                    this->x--;
                    return true;
                }
                break;
            case direction::right:
                if (this->x + this->xmax < board_size.first - 1 && this->brd->collides(this->clone(this->x + 1, this->y)) == false)
                {
                    this->x++;
                    return true;
                }
                break;
            case direction::down:
                if (this->y + this->ymax < board_size.second - 1 && this->brd->collides(this->clone(this->x, this->y + 1)) == false)
                {
                    this->y++;
                    return true;
                }
                break;
        };
        return false;
    }
    void piece::rotate()
    {
        size_t N = this->size();

        auto cln = *this;
        auto &ref = cln.shape;
        for (size_t sl = 0; sl < N / 2; sl++)
        {
            for (size_t i = 0; i < N - 1 - sl * 2; i++)
            {
                auto temp                       = ref[sl][sl + i];
                ref[sl][sl + i]                 = ref[sl + i][N - 1 - sl];
                ref[sl + i][N - 1 - sl]         = ref[N - 1 - sl][N - 1 - sl - i];
                ref[N - 1 - sl][N - 1 - sl - i] = ref[N - 1 - sl - i][sl];
                ref[N - 1 - sl - i][sl]         = temp;
            }
        }
        cln.getexts();

        if (this->brd->collides(cln) == false)
            *this = cln;
    }

    static const auto pieces = frozen::make_map<ids, const piece>(
    {
        { ids::I, {
            ids::I, startx - 2, starty - 1, {
                { 0, 0, 0, 0 },
                { 1, 1, 1, 1 },
                { 0, 0, 0, 0 },
                { 0, 0, 0, 0 }
            }
        } },
        { ids::O, {
            ids::O, startx - 2, starty - 1, {
                { 0, 0, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 1, 1, 0 },
                { 0, 0, 0, 0 }
            }
        } },
        { ids::Sr, {
            ids::Sr, startx - 1, starty - 1, {
                { 0, 0, 0 },
                { 0, 1, 1 },
                { 1, 1, 0 }
            }
        } },
        { ids::Sl, {
            ids::Sl, startx - 1, starty - 1, {
                { 0, 0, 0 },
                { 1, 1, 0 },
                { 0, 1, 1 }
            }
        } },
        { ids::Lr, {
            ids::Lr, startx - 1, starty - 1, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 0, 0, 1 }
            }
        } },
        { ids::Ll, {
            ids::Ll, startx - 1, starty - 1, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 1, 0, 0 }
            }
        } },
        { ids::T, {
            ids::T, startx - 1, starty - 1, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 0, 1, 0 }
            }
        } }
    });

    bool board::add_piece(ids id)
    {
        if (this->current_piece)
            return false;

        auto pc = pieces.at(id);
        if (this->collides(pc))
            return false;

        auto ret = std::make_unique<piece>(pc);
        ret->brd = this;
        this->current_piece = std::move(ret);
        return true;
    }

    bool board::collides(piece pc)
    {
        for (auto y = pc.y + pc.ymin; y <= pc.y + pc.ymax; y++)
        {
            if (y < 0)
                continue;
            for (auto x = pc.x + pc.xmin; x <= pc.x + pc.xmax; x++)
            {
                if (x < 0 || x >= board_size.first)
                    return true;

                if (pc.getat(x, y) == used && this->array[y][x] == used)
                    return true;
            }
        }
        return false;
    }

    postype board::linefull()
    {
        for (auto y = board_size.second - 1; y >= 0; y--)
        {
            for (auto x = 0; x < board_size.first; x++)
            {
                if (this->array[y][x] == unused)
                    goto nope;
            }
            return y;
            nope:;
        }
        return -1;
    }

    void board::linedel(postype ly)
    {
        assert(ly < board_size.second);
        for (auto y = ly; y > 0; y--)
            std::copy(this->array[y - 1].begin(), this->array[y - 1].end(), this->array[y].begin());
    }

    void board::copy()
    {
        if (!this->current_piece)
            return;

        auto &ref = this->current_piece;
        auto py = ref->y;
        auto px = ref->x;

        for (auto y = py + ref->ymin; y <= py + ref->ymax; y++)
        {
            if (y < 0)
                continue;
            for (auto x = px + ref->xmin; x <= px + ref->xmax; x++)
            {
                if (x < 0)
                    continue;
                if (this->current_piece->getat(x, y))
                    this->array[y][x] = used;
            }
        }
        this->current_piece.reset();
    }

    void board::draw()
    {
        for (auto y = 0; y < board_size.second; y++)
        {
            fmt::print(_TETRIS_COLOUR "<!");
            for (auto x = 0; x < board_size.first; x++)
            {
                bool usd = this->array[y][x];
                if (this->current_piece)
                    usd = usd || this->current_piece->getat(x, y);
                fmt::print(_TETRIS_COLOUR "{}", usd ? "[]" : " .");
            }
            fmt::print(_TETRIS_COLOUR "!>\n");
        }
        fmt::print(_TETRIS_COLOUR "<!{:=>{}}!>\n", "", board_size.first * 2);
    }

    ids game::genid()
    {
        auto ret = static_cast<ids>(this->uid(this->rnd));
        while (ret == this->lastid)
            ret = static_cast<ids>(this->uid(this->rnd));

        this->lastid = ret;
        return ret;
    }

    void game::start()
    {
        using namespace std::chrono_literals;
        auto delay = 500ms;

        auto end = std::chrono::steady_clock::now() + delay;
        assert(this->brd.add_piece(this->genid()));

        this->redraw();

        while (true)
        {
            if (this->keep_running == false)
                break;

            bool speedup = false;

            auto action = this->trm.getkey();
            switch (action)
            {
                case term::action::move_right:
                    this->brd.current_piece->move(direction::right);
                    this->redraw();
                    break;
                case term::action::move_left:
                    this->brd.current_piece->move(direction::left);
                    this->redraw();
                    break;
                case term::action::speedup:
                    speedup = true;
                    break;
                case term::action::rotate:
                    this->brd.current_piece->rotate();
                    this->redraw();
                    break;
                case term::action::exit:
                    return;
                default:
                    break;
            }

            auto now = std::chrono::steady_clock::now();
            if (now < (speedup ? end - (delay - 25ms) : end))
                continue;
            end = now + delay;

            if (this->brd.current_piece && this->brd.current_piece->move(direction::down) == false)
            {
                this->brd.copy();
                if (this->brd.add_piece(this->genid()) == false)
                    break;
            }

            static auto oldscore = 0;
            if (this->score >= (oldscore + 100) && delay > 50ms)
            {
                oldscore = this->score;
                delay -= 50ms;
            }
            this->redraw();
        }
    }

    void game::redraw()
    {
        this->trm.reset();

        postype ly = -1;
        while ((ly = this->brd.linefull()) != -1)
        {
            this->brd.linedel(ly);
            this->score += ly; // TODO: <-- incorrect
        }

        fmt::print(_TETRIS_COLOUR "<!{: ^{}}!>\n", fmt::format("Score: {}", this->score), board_size.first * 2);
        this->brd.draw();
    }
} // namespace tetris