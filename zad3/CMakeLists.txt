cmake_minimum_required(VERSION 3.0)
project(zad3)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE on)
#set(CMAKE_C_STANDARD 99)
set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")

set(SOURCE_FILES main.c)
add_executable(zad3 ${SOURCE_FILES})