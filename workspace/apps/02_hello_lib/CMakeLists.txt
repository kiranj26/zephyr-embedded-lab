cmake_minimum_required(VERSION 3.20.0)

# Loads Zephyr's build system; this also creates the "app" library target
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(hello_lib)

# Build lib/say_hello, which defines the say_hello target (must come before linking it)
add_subdirectory(lib/say_hello)

target_sources(app PRIVATE src/main.c)

# app is the final consumer, so nothing needs to propagate further: PRIVATE
target_link_libraries(app PRIVATE say_hello)
