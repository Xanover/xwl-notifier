cmake_minimum_required(VERSION 4.0)
project(xwl_notifier C)

set(CMAKE_C_STANDARD 23)

find_package(X11 REQUIRED)

add_executable(xwl_notifier main.c
)

target_link_libraries(xwl_notifier PRIVATE X11::X11)
