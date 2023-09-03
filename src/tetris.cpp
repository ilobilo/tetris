// Copyright (C) 2022  ilobilo

#include <fmt/ranges.h>
#include <tetris.hpp>
#include <chrono>

namespace tetris
{
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
            ids::I, {
                { 0, 0, 0, 0 },
                { 1, 1, 1, 1 },
                { 0, 0, 0, 0 },
                { 0, 0, 0, 0 },
            }
        } },
        { ids::O, {
            ids::O, {
                { 0, 0, 0, 0 },
                { 0, 1, 1, 0 },
                { 0, 1, 1, 0 },
                { 0, 0, 0, 0 },
            }
        } },
        { ids::Sr, {
            ids::Sr, {
                { 0, 0, 0 },
                { 0, 1, 1 },
                { 1, 1, 0 },
            }
        } },
        { ids::Sl, {
            ids::Sl, {
                { 0, 0, 0 },
                { 1, 1, 0 },
                { 0, 1, 1 },
            }
        } },
        { ids::Lr, {
            ids::Lr, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 0, 0, 1 },
            }
        } },
        { ids::Ll, {
            ids::Ll, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 1, 0, 0 },
            }
        } },
        { ids::T, {
            ids::T, {
                { 0, 0, 0 },
                { 1, 1, 1 },
                { 0, 1, 0 },
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
        ret->setboard(this);
        this->current_piece = std::move(ret);
        return true;
    }

    bool board::collides(piece pc)
    {
        auto px = pc.getx();
        auto py = pc.gety();

        for (auto y = py + pc.getymin(); y <= py + pc.getymax(); y++)
        {
            for (auto x = px + pc.getxmin(); x <= px + pc.getxmax(); x++)
            {
                if (x < 0 || x >= board_size.first)
                    return true;

                if (pc.getat(x, y) == used && this->array[y][x] == used)
                    // if (pc.getat(x, y - 1) != used)
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
        auto py = ref->gety();
        auto px = ref->getx();

        for (auto y = py + ref->getymin(); y <= py + ref->getymax(); y++)
        {
            if (y < 0)
                continue;
            for (auto x = px + ref->getxmin(); x <= px + ref->getxmax(); x++)
            {
                if (x < 0)
                    continue;
                if (this->current_piece->getat(x, y))
                    this->array[y][x] = true;
            }
        }
        this->current_piece.reset();
    }

    void board::draw()
    {
        for (auto y = 0; y < board_size.second; y++)
        {
            fmt::print("<!");
            for (auto x = 0; x < board_size.first; x++)
            {
                bool usd = this->array[y][x];
                if (this->current_piece)
                    usd = usd || this->current_piece->getat(x, y);
                fmt::print("{}", usd ? "[]" : " .");
            }
            fmt::println("!>\r");
        }
        fmt::println("<!{:=>{}}!>\r", "", board_size.first * 2);
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
        constexpr auto delay = 500ms;

        auto end = std::chrono::steady_clock::now() + delay;
        this->brd.add_piece(this->genid());
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

        fmt::println("<!{: ^{}}!>\r", fmt::format("Score: {}", this->score), board_size.first * 2);
        this->brd.draw();
    }
} // namespace tetris