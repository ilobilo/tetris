# Tetris
Small Tetris clone in C++ with [Original Rotation System](https://tetris.wiki/Original_Rotation_System).\
Scoring system is a bit wacky. (It's a feature, not a bug!)

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