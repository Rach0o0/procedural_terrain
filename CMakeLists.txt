cmake_minimum_required(VERSION 3.16)
project(procedural_terrain LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(terrain
    src/main.cpp
    src/image_io.cpp
    src/perlin.cpp
)

target_include_directories(terrain PRIVATE src)
target_include_directories(terrain SYSTEM PRIVATE external)

if(MSVC)
    target_compile_options(terrain PRIVATE /W4)
else()
    target_compile_options(terrain PRIVATE -Wall -Wextra)
endif()