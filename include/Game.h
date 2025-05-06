#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "Tower.h"
#include "Enemy.h"
#include "Projectile.h"

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
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;

    TowerType selectedTowerType = TowerType::Archer; // Tipo de torre seleccionado

    void handleEvents();
    void update();
    void render();
    void spawnEnemy();

    void renderMap(); // Nueva función para renderizar el mapa
    void placeTower(int mouseX, int mouseY);
};
