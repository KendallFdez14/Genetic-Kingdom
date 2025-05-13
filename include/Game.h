#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Tower.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Astar.h"
#include "WaveManager.h"
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
    int waveTimer;
    float timeScale; // Time scale factor for debugging

    std::vector<Tower> towers;
    std::vector<std::unique_ptr<Enemy>> enemies; // Cambiado a punteros inteligentes
    std::vector<std::unique_ptr<Projectile>> projectiles;

    TowerType selectedTowerType; // Tipo de torre seleccionado

    void handleEvents();
    void update();
    void render();

    void renderMap();
    void renderPannel();
    void placeTower(int mouseX, int mouseY);

    int gold;
    int mapSize;

    WaveManager waveManager;
};
