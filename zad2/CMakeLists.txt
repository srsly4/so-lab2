cmake_minimum_required(VERSION 3.6)
project(zad2)

#set(CMAKE_C_STANDARD 99)
set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_FILES main.c)
add_definitions("-Wall")
add_executable(zad2 ${SOURCE_FILES})

add_executable(zad2-nftw ${SOURCE_FILES})
target_compile_definitions(zad2-nftw PRIVATE USE_NFTW=true)