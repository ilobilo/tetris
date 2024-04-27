// Copyright (C) 2024 ilobilo

#pragma once

#include <glm/ext.hpp>
#include <glm/ext/matrix_integer.hpp>

#include <type_traits>
#include <algorithm>
#include <cstddef>

namespace tetris
{
    using ssize_t = std::make_signed_t<std::size_t>;

    class piece
    {
        private:
        glm::i8mat4 data;
        std::size_t size;

        constexpr void calculate_extents()
        {
            this->xmin = this->xmax = 0;
            this->ymin = this->ymax = 0;

            bool findymin = true;
            bool findxmin = true;

            for (std::size_t yi = 0; yi < this->size; yi++)
            {
                for (std::size_t xi = 0; xi < this->size; xi++)
                {
                    if (this->at(xi, yi))
                    {
                        if (findymin)
                        {
                            findymin = false;
                            ymin = yi;
                        }

                        if (findxmin)
                        {
                            findxmin = false;
                            xmin = xi;
                        }
                        else xmin = std::min(xmin, ssize_t(xi));

                        xmax = std::max(xmax, ssize_t(xi));
                        ymax = yi;
                    }
                }
            }
        }

        public:
        ssize_t xmin, xmax;
        ssize_t ymin, ymax;

        ssize_t x;
        ssize_t y;

        constexpr piece(const glm::i8mat4 cdata, std::size_t size) :
            data { cdata }, size { size },
            xmin { 0 }, xmax { 0 }, ymin { 0 }, ymax { 0 },
            x { 0 }, y { 0 }
        {
            this->calculate_extents();
        }

        void rotate()
        {
            this->data = glm::transpose(this->data);
            for (std::size_t i = 0; i < this->size; i++)
            {
                auto ptr = glm::value_ptr(this->data[i]);
                std::reverse(ptr, ptr + this->size);
            }
            this->calculate_extents();
        }

        constexpr std::size_t get_size() const { return this->size; }

        constexpr bool at(ssize_t x, ssize_t y) const
        {
            const ssize_t sz = this->size;
            if (x >= sz || y >= sz)
                return false;
            return this->data[y][x] == 1;
        }
    };

    inline constexpr piece pieces[]
    {
        // I
        { {
            { 0, 0, 0, 0 },
            { 1, 1, 1, 1 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 4 },
        // J
        { {
            { 1, 0, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 3 },
        // L
        { {
            { 0, 0, 1, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 3 },
        // O
        { {
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 2 },
        // S
        { {
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 3 },
        // T
        { {
            { 0, 1, 0, 0 },
            { 1, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 3 },
        // Z
        { {
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }, 3 }
    };
} // namespace tetris
