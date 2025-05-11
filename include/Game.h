#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Tower.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Astar.h"

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void clean();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;

    std::vector<std::vector<int>> map; // Representación del mapa
    std::vector<std::pair<int, int>> path; // Lista de nodos del camino
    int enemyTimer;

    std::vector<Tower> towers;
    std::vector<std::unique_ptr<Enemy>> enemies; // Cambiado a punteros inteligentes
    std::vector<std::unique_ptr<Projectile>> projectiles;

    TowerType selectedTowerType; // Tipo de torre seleccionado

    int mouseX = 0; // Posición X del mouse
    int mouseY = 0; // Posición Y del mouse

    void handleEvents();
    void update();
    void render();
    void spawnEnemy();

    void renderMap();
    void renderPannel();
    void placeTower(int mouseX, int mouseY);

    int gold;
    int mapSize;
};
