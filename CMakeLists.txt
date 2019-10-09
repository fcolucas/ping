cmake_minimum_required(VERSION 3.5)
project(ping)

set(CMAKE_CXX_STANDARD 99)
# this executable is deprecated
add_executable(ping
        src/protocol.c lib/protocol.h src/echo.c lib/communication.h src/communication.c)
