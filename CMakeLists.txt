cmake_minimum_required(VERSION 3.14)
project(CMPT433ASS2 C)

#set(CMAKE_C_STANDARD 99)
SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/cmpt433/public")

add_executable(Sorter
        udp_listen.c
        main.c
        a2d.c
        I2C.c
        sorter.c
        change_digits.c change_digits.h Utility.c Utility.h)


target_link_libraries(Sorter pthread)
