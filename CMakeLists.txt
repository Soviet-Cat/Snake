cmake_minimum_required(VERSION 3.31)
project(Snake)

set(CMAKE_CXX_STANDARD 11)

# load sdl2
set(SDL2_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2/include")
set(SDL2_LIB_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2/lib")
set(SDL2_BIN_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2/bin")
find_library(SDL2_LIBRARY NAMES SDL2 PATHS ${SDL2_LIB_DIR})
include_directories(${SDL2_INCLUDE_DIR})

add_executable(Snake main.cpp)

# link sdl2
target_link_libraries(Snake ${SDL2_LIBRARY})
add_custom_command(TARGET Snake POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${SDL2_BIN_DIR}/SDL2.dll $<TARGET_FILE_DIR:Snake>
)
