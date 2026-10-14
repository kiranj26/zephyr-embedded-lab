cmake_minimum_required(VERSION 3.20.0)

# Part 2 of the lesson: point Zephyr at YOUR module (copy modules/_templates/kconfig_module -> modules/greeting)
# set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/greeting")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(kconfig_random)

target_sources(app PRIVATE src/main.c)
