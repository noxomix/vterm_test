cmake_minimum_required(VERSION 3.27)
project(vterm_test C)

set(CMAKE_C_STANDARD 17)

add_executable(vterm_test main.c
        term_parts/fps_counter.h
        term_parts/pty_manager.h
        term_parts/application.h
        term_parts/terminal.h
        ../../../../sdl_term.h
)
