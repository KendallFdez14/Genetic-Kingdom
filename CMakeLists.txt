cmake_minimum_required(VERSION 3.10)
project(Genetic-Kingdom)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Enemy.cpp
    src/Tower.cpp
    src/Projectile.cpp
)

add_executable(Genetic-Kingdom ${SOURCES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(Genetic-Kingdom ${SDL2_LIBRARIES})
