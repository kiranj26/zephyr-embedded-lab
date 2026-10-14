# TODO 2: create a STATIC library target from src/say_hello.c
#         - add_library(...)
#         - target_include_directories(... PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
#         - target_link_libraries(... PRIVATE zephyr_interface)   # why is this needed? (compile flags/arch!)
