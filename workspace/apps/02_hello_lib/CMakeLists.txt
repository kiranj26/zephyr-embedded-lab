cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_lib)

# Lesson 2 challenge: turn lib/say_hello into a real CMake library and link it.
# TODO 1: add_subdirectory(lib/say_hello)
# TODO 3: target_link_libraries(app PRIVATE <your library target>)

target_sources(app PRIVATE src/main.c)
