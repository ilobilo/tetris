# Tetris
Small Tetris clone in C++

## Controls
* `a`/`left`: Move left
* `d`/`right`: Move right
* `w`/`up`: Rotate
* `s`/`down`: Soft drop
* `space`: Hard drop
* `q`/`x`: Quit

## Building and Running
Required: C++20 compiler, `meson`, `ninja`, `libncurses`

* `meson setup builddir`
* `ninja -C builddir`
* `./builddir/tetris`