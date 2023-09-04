# Tetris
Small Tetris clone in C++ with [Original Rotation System](https://tetris.wiki/Original_Rotation_System).\
Scoring system is a bit wacky. (It's a feature, not a bug!)\
At the start Tetrominos fall every `500ms`. Game will gradually speed up in every `+100 points` by `50ms` until delay goes all the way down to `50ms`.

## Controls
* `a`/`left`: Move left
* `d`/`right`: Move right
* `w`/`up`: Rotate
* `s`/`down`: Speed up
* `q`/`x`: Quit

## Building and Running
You will need to have C++ compiler, `meson` and `ninja` installed.
* `meson setup builddir`
* `ninja -C builddir`
* `./builddir/tetris`

You can also add `-Dgreen_foreground=true` to `meson setup` to use green characters instead of white, like original Tetris.