cmake_minimum_required(VERSION 3.13)
project(sobel-filter C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-O2 -Wall -fsanitize=address -fsanitize=leak -g")

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
file(GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable(sobel-filter main.c ${SRC_FILES} ${INCLUDE_FILES})