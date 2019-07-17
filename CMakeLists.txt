cmake_minimum_required(VERSION 3.14)
project(CMPT433ASS2 C)

set(CMAKE_C_STANDARD 99)
#SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)

add_executable(CMPT433ASS2
        udp_listen.c
        main.c
        )


target_link_libraries(CMPT433ASS2 pthread)
