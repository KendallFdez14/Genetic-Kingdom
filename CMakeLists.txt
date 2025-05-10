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
    src/Astar.cpp
    src/WaveManager.cpp
    src/GeneticAlgorithm.cpp
)

add_executable(Genetic-Kingdom ${SOURCES})

# Agregar SDL2 y SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Incluir directorios de SDL2, SDL2_image y SDL2_ttf
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# Enlazar SDL2 y SDL2_image manualmente
target_link_libraries(Genetic-Kingdom ${SDL2_LIBRARIES} -lSDL2_image -lSDL2_ttf)
