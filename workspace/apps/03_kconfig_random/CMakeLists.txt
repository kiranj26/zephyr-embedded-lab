cmake_minimum_required(VERSION 3.20.0)

# Must be set BEFORE find_package(Zephyr): Zephyr reads its module list (and each
# module's Kconfig) while find_package runs, so anything added later is never seen.
set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/greeting")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(kconfig_random)

target_sources(app PRIVATE src/main.c)
