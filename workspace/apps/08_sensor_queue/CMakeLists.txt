cmake_minimum_required(VERSION 3.20.0)

# Reuse YOUR lesson-6 driver module (it must live under workspace/modules/ so the container can see it).
# set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/mcp9808_mine")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sensor_queue)

target_sources(app PRIVATE src/main.c)
