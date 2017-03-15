cmake_minimum_required(VERSION 3.0)
project(zad1)

set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")

set(SOURCE_FILES src/main.c src/sysoperations.c include/sysoperations.h src/liboperations.c include/liboperations.h)
add_executable(zad1 ${SOURCE_FILES})

add_custom_target(tests
        COMMAND sh ./tests.sh
        DEPENDS zad1
        )