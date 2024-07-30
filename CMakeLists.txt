cmake_minimum_required(VERSION 3.10)
project(ParticleSim)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)


find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(ParticleSim particleSim.c)
target_link_libraries(ParticleSim ${SDL2_LIBRARIES})
target_link_libraries(ParticleSim m)
