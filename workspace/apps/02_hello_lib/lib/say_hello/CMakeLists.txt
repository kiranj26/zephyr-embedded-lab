# say_hello: a minimal static library linked into the Zephyr app

add_library(say_hello STATIC src/say_hello.c)

# PUBLIC: the header is part of the library's interface, so anything that links
# say_hello (here: app) gets this include path too
target_include_directories(say_hello PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# zephyr_interface carries Zephyr's include paths, -imacros autoconf.h (CONFIG_*),
# defines and CPU flags (e.g. -mlongcalls on Xtensa). Without it say_hello.c can't
# find <zephyr/kernel.h> and would be built with the wrong flags.
# PRIVATE because say_hello.h itself uses no Zephyr headers; make it PUBLIC if it ever does.
target_link_libraries(say_hello PRIVATE zephyr_interface)
