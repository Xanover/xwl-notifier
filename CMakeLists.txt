cmake_minimum_required(VERSION 3.10)
project(xwl_notifier C)

set(CMAKE_C_STANDARD 23)

find_package(X11 REQUIRED)

add_executable(xwl_notifier src/main.c)

install(TARGETS xwl_notifier DESTINATION $ENV{HOME}/.local/bin)

target_link_libraries(xwl_notifier PRIVATE X11::X11)

install(FILES systemd/xwl-notifier.service DESTINATION $ENV{HOME}/.config/systemd/user)
