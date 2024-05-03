// Copyright (C) 2024 ilobilo

#include <tetris.hpp>
#include <chrono>
#include <cmath>

namespace tetris
{
    static constexpr std::size_t required_width = 1 + (hsquares * chars::nch) + 1;
    static constexpr auto softdrop_delay = std::chrono::milliseconds(10);

    // this is terrible
    std::size_t board_t::random()
    {
        static constexpr std::size_t pool[] { 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6, 0, 1, 2, 3, 4, 5, 6 };

        std::size_t ret = 0;
        do {
            ret = pool[this->dist(this->mt)];
        } while (ret == this->history[0] || ret == this->history[1] || ret == this->history[2] || ret == this->history[3]);

        std::memmove(this->history + 1, this->history, sizeof(this->history) - sizeof(std::size_t));
        return this->history[0] = ret;
    }

    void board_t::add_piece()
    {
        if (this->falling == true)
            return;

        auto &pc = (this->current_piece = pieces[this->random()]);
        this->current_piece.x = (hsquares / 2) - (pc.get_size() / 2);
        this->current_piece.y = -2;
        this->falling = true;
    }

    std::size_t board_t::clearlines()
    {
        std::size_t lines = 0;
        for (std::size_t i = 0; auto &line : this->buffer)
        {
            if (std::all_of(line.begin(), line.end(), [](auto a) { return a != 0; }))
            {
                std::memmove(std::next(this->buffer.begin(), 1), this->buffer.begin(), sizeof(this->buffer[0]) * i);
                lines++;
            }
            i++;
        }
        return lines;
    }

    bool board_t::move(movements mv)
    {
        if (this->falling == false)
            return true;

        auto pc = this->current_piece;
        switch (mv)
        {
            case movements::left:
                if (pc.x + pc.xmin > 0)
                    pc.x--;
                break;
            case movements::right:
                if ((pc.x + pc.xmax) < ssize_t(hsquares))
                    pc.x++;
                break;
            case movements::rotate:
            {
                auto tmp = pc;
                tmp.rotate();
                if (this->check(tmp) == false)
                {
                    if (pc.y + pc.ymin > 0)
                    {
                        // pc.y--;
                        // pc.rotate();

                        tmp = pc;
                        tmp.y--;
                        tmp.rotate();
                        if (this->check(tmp) == false)
                        {
                            if ((tmp.x + tmp.xmin) < 0)
                                pc.x -= (tmp.x + tmp.xmin);
                            else if ((tmp.x + tmp.xmax) >= ssize_t(hsquares - 1))
                                pc.x--;

                            pc.rotate();
                        }
                        else pc = tmp;
                    }
                }
                else pc = tmp;
                break;
            }
            case movements::down:
                if ((pc.y + pc.ymax) < ssize_t(vsquares))
                    pc.y++;
                break;
        }

        if (this->check(pc))
        {
            this->current_piece = pc;
            { // ghost piece
                piece tmp { };
                do {
                    tmp = pc;
                    pc.y++;
                } while ((pc.y + pc.ymax) < ssize_t(vsquares) && this->check(pc));

                this->ghost = tmp;
            }
            return false;
        }
        pc = this->current_piece;

        if (mv == movements::down)
        {
            if (pc.y < 0)
                return true;

            this->falling = false;
            for (auto y = (pc.y + pc.ymin); y < (pc.y + pc.ymax + 1); y++)
            {
                for (auto x = (pc.x + pc.xmin); x < (pc.x + pc.xmax + 1); x++)
                {
                    if (x < 0 || y < 0)
                        continue;

                    if (pc.at(x - pc.x, y - pc.y))
                        this->buffer[y][x] = pc.colour;
                }
            }
        }
        return false;
    }

    bool board_t::check(const piece pc) const
    {
        if (((pc.x + pc.xmax) >= ssize_t(hsquares)) || ((pc.y + pc.ymax) >= ssize_t(vsquares)))
            return false;

        if ((pc.x + pc.xmin) < 0)
            return false;

        for (auto y = (pc.y + pc.ymin); y < (pc.y + pc.ymax + 1); y++)
        {
            for (auto x = (pc.x + pc.xmin); x < (pc.x + pc.xmax + 1); x++)
            {
                if (y < 0)
                    continue;

                if (x < 0 || (this->buffer[y][x] && pc.at(x - pc.x, y - pc.y)))
                    return false;
            }
        }
        return true;
    }

    void board_t::draw(std::string_view title) const
    {
        auto printat = [&](auto x, auto y, auto str, std::size_t colour = 8)
        {
            this->term.printat(x, y, str, colour);
        };

        // board
        {
            // left and right borders
            for (std::size_t y = 1; y < (vsquares + 1); y++)
            {
                printat(0, y, chars::border::vertical);
                printat(required_width - 1, y, chars::border::vertical);
            }

            // top and bottom borders
            const auto y = vsquares + 1;

            printat(0, 0, chars::border::corners::topleft);
            printat(0, y, chars::border::corners::botleft);

            for (std::size_t x = 1; x < (required_width - 1); x++)
            {
                printat(x, 0, chars::border::horizontal);
                printat(x, y, chars::border::horizontal);
            }

            printat(required_width - 1, 0, chars::border::corners::topright);
            printat(required_width - 1, y, chars::border::corners::botright);

            // title
            assert((title.length() - 2) <= (required_width - 2));
            printat((required_width / 2) - (title.length() / 2), 0, title);
        }

        // pieces
        for (std::size_t y = 0; y < vsquares; y++)
        {
            for (std::size_t x = 0; x < hsquares; x++)
            {
                const auto &cp = this->current_piece;
                const ssize_t cps = cp.get_size();

                auto is_current_falling = [this, cps, x, y](const auto &cp)
                {
                    return this->falling &&
                        (cp.x <= ssize_t(x) && ssize_t(x) < (cp.x + cps)) &&
                        (cp.y <= ssize_t(y) && ssize_t(y) < (cp.y + cps)) &&
                        (cp.at(x - cp.x, y - cp.y));
                };

                if (is_current_falling(cp))
                    printat(x * chars::nch + 1, y + 1, chars::falling, cp.colour);
                else if (this->buffer[y][x])
                    printat(x * chars::nch + 1, y + 1, chars::full, this->buffer[y][x]);
                else if (is_current_falling(this->ghost))
                    printat(x * chars::nch + 1, y + 1, chars::ghost, 3);
            }
        }
    }

    void game::start()
    {
        auto get_delay = [&]
        {
            return std::chrono::milliseconds(static_cast<std::size_t>(std::pow(0.8 - ((this->level - 1) * 0.007), this->level - 1) * 1000));
        };

        auto delay = get_delay();
        bool paused = false;

        auto update = [&]
        {
            this->lines += this->board.clearlines();

            auto oldlvl = this->level;
            while (this->lines >= 10)
            {
                this->lines -= 10;
                this->level++;
            }
            if (this->level != oldlvl)
                delay = get_delay();

            this->term.clear();
            this->board.draw(paused ? " Paused " : " Level: " + std::to_string(this->level) + " - " + std::to_string(this->lines) + "/10 ");
            this->term.refresh();
        };

        this->term.init();
        this->board.add_piece();
        update();

        auto end = std::chrono::steady_clock::now() + delay;

        while (this->game_over == false)
        {
            auto now = std::chrono::steady_clock::now();

            bool softdrop = false;
            bool should_update = false;

            if (this->board.is_falling() == false)
                this->board.add_piece();

            auto key = this->term.getkey();
            switch (key)
            {
                case 'P':
                case 'p':
                    paused = !paused;
                    goto update;
                case 'q':
                case 'Q':
                case 'x':
                case 'X':
                    goto exit;
                default:
                    break;
            }
            if (paused == true)
                continue;

            switch (key)
            {
                case KEY_LEFT:
                case 'a':
                    this->board.move(movements::left);
                    should_update = true;
                    break;
                case KEY_RIGHT:
                case 'd':
                    this->board.move(movements::right);
                    should_update = true;
                    break;
                case KEY_UP:
                case 'w':
                    this->board.move(movements::rotate);
                    should_update = true;
                    break;
                case KEY_DOWN:
                case 's':
                    softdrop = true;
                    break;
                case ' ':
                    while (this->board.is_falling() == true)
                    {
                        if (this->board.move(movements::down))
                        {
                            this->game_over = true;
                            goto update;
                        }
                    }
                    should_update = true;
                    break;
                default:
                    break;
            }

            if (now < (softdrop ? end - (delay - softdrop_delay) : end))
            {
                if (should_update)
                    goto update;
                continue;
            }
            end = now + delay;

            if (this->board.is_falling() == true)
                if (this->board.move(movements::down))
                    this->game_over = true;

            update:
            update();
        }
        exit:
        this->term.deinit();
    }
} // namespace tetris
