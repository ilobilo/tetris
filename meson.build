# Copyright (C) 2024  ilobilo

project('Tetris', 'cpp',
    version : 'v0.1',
    default_options : [
        'cpp_std=c++20'
    ]
)

sources = files(
    'source/main.cpp',
    'source/tetris.cpp'
)

include = include_directories('source')

executable('tetris',
    dependencies : [
        dependency('curses'),
        dependency('glm')
    ],
    sources : sources,
    include_directories : include
)