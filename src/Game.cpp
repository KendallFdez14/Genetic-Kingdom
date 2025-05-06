#include "Game.h"
#include <iostream>
#include <algorithm>
#include <SDL2/SDL_image.h> // Include SDL_image for IMG_LoadTexture

SDL_Texture* ogreTexture = nullptr;
SDL_Texture* darkElfTexture = nullptr;
SDL_Texture* harpyTexture = nullptr;
SDL_Texture* mercenaryTexture = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), running(false), enemyTimer(0) {
    // Inicializar el mapa 8x8 con un camino
    map = {
        {1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1}
    };

    // Definir el camino como una lista de nodos
    path  = {
        {0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4},
        {4, 4}, {5, 4}, {6, 4}, {7, 4}, {7, 5}, {7, 6}, {7, 7}
    };
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        return false;
    }

    // Cargar texturas
    ogreTexture = IMG_LoadTexture(renderer, "/home/kendall/Progra/Genetic-Kingdom/assets/ogre.png");
    darkElfTexture = IMG_LoadTexture(renderer, "/home/kendall/Progra/Genetic-Kingdom/assets/dark_elf.png");
    harpyTexture = IMG_LoadTexture(renderer, "/home/kendall/Progra/Genetic-Kingdom/assets/harpy.png");
    mercenaryTexture = IMG_LoadTexture(renderer, "/home/kendall/Progra/Genetic-Kingdom/assets/mercenary.png");

    if (!ogreTexture || !darkElfTexture || !harpyTexture || !mercenaryTexture) {
        std::cerr << "Error al cargar texturas: " << SDL_GetError() << "\n";
        return false;
    }

    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                placeTower(event.button.x, event.button.y);
            }
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_1:
                    selectedTowerType = TowerType::Archer;
                    break;
                case SDLK_2:
                    selectedTowerType = TowerType::Mage;
                    break;
                case SDLK_3:
                    selectedTowerType = TowerType::Artillery;
                    break;
            }
        }
    }
}

void Game::placeTower(int mouseX, int mouseY) {
    int col = mouseX / 75;
    int row = mouseY / 75;

    if (row >= 0 && row < 8 && col >= 0 && col < 8 && map[row][col] == 0) {
        // Coloca una torre del tipo seleccionado en la celda
        towers.emplace_back(col * 75 + 37, row * 75 + 37, selectedTowerType);
        map[row][col] = 2; // Marca la celda como ocupada
    }
}

void Game::update() {
    enemyTimer++;
    if (enemyTimer >= 120) { // Cada 2 segundos
        spawnEnemy();
        enemyTimer = 0;
    }

    for (auto& tower : towers)
        tower.update(enemies, projectiles);

        for (auto& enemy : enemies)
        enemy->update(); // Llamar a update() en el puntero

    for (auto& projectile : projectiles)
        projectile->update(); // Llamar a update() en el puntero

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }), enemies.end());

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p) { return p->hasHit(); }), projectiles.end());
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    renderMap(); // Renderizar el mapa

    for (auto& tower : towers)
        tower.render(renderer);

        for (auto& enemy : enemies)
        enemy->render(renderer); // Llamar a render() en el puntero

    for (auto& projectile : projectiles)
        projectile->render(renderer); // Llamar a render() en el puntero

    SDL_RenderPresent(renderer);
}

void Game::renderMap() {
    int cellSize = 75; // Tamaño de cada celda (800px / 8 = 100px por celda)
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (map[row][col] == 1) {
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Color del camino
            } else {
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Color del espacio vacío
            }
            SDL_Rect cell = {col * cellSize, row * cellSize, cellSize, cellSize};
            SDL_RenderFillRect(renderer, &cell);
        }
    }
}

void Game::spawnEnemy() {
    // Alternar entre diferentes tipos de enemigos
    static int enemyType = 0;

    switch (enemyType) {
        case 0:
            enemies.emplace_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
            break;
        case 1:
            enemies.emplace_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
            break;
        case 2:
            enemies.emplace_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
            break;
        case 3:
            enemies.emplace_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
            break;
    }

    enemyType = (enemyType + 1) % 4; // Alternar entre los tipos de enemigos
}

void Game::clean() {
    // Liberar texturas
    if (ogreTexture) SDL_DestroyTexture(ogreTexture);
    if (darkElfTexture) SDL_DestroyTexture(darkElfTexture);
    if (harpyTexture) SDL_DestroyTexture(harpyTexture);
    if (mercenaryTexture) SDL_DestroyTexture(mercenaryTexture);

    // Liberar el renderer y la ventana
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    SDL_Quit();
}
