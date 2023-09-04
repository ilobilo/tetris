project('Tetris', 'cpp',
    version : 'v0.1',
    default_options : ['cpp_std=c++20']
)

sources = files(
    'src/main.cpp',
    'src/term.cpp',
    'src/tetris.cpp'
)

includes = include_directories('src')

cmake = import('cmake')

fmt_opts = cmake.subproject_options()
fmt_opts.add_cmake_defines({ 'FMT_INSTALL' : false })
fmt_dep = cmake.subproject('fmt', options : fmt_opts).dependency('fmt')
frozen_dep = cmake.subproject('frozen').dependency('frozen')

if get_option('green_foreground')
    add_project_arguments('-DTETRIS_GREEN_TEXT', language : 'cpp')
endif

executable('tetris',
    dependencies : [
        fmt_dep, frozen_dep
    ],
    sources : sources,
    include_directories : includes
)