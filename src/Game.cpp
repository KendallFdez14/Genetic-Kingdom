#include "Game.h"
#include <iostream>
#include <algorithm>
#include <SDL2/SDL_image.h> // Include SDL_image for IMG_LoadTexture
#include <SDL2/SDL_ttf.h> // Include SDL_ttf for TTF_RenderText_Blended
#include <sstream>
#include <iomanip> // For std::fixed and std::setprecision
#include <numeric>
#include "Tower.h"
#include "Astar.h"
#include "WaveManager.h"

SDL_Texture* ogreTexture = nullptr;
SDL_Texture* darkElfTexture = nullptr;
SDL_Texture* harpyTexture = nullptr;
SDL_Texture* mercenaryTexture = nullptr;
TTF_Font* font = nullptr;

Game::Game() : window(nullptr), renderer(nullptr), running(false), enemyTimer(0), gold(50), selectedTowerType(TowerType::Archer), waveManager(std::vector<std::pair<int, int>>(), nullptr, nullptr, nullptr, nullptr) {
    // Inicializar el mapa 16x16 con un camino
    map = {
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}
    };
    mapSize = map.size();

    // Definir el camino como una lista de nodos
    path = aStarSearch(map, {0, 0}, {15, 14}); // Llamar a la función A* para obtener el camino
}

Game::~Game() {
    clean();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow("Tower Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mapSize * 75 + 500, mapSize * 75, 0);
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
    ogreTexture = IMG_LoadTexture(renderer, "../assets/ogre.png");
    darkElfTexture = IMG_LoadTexture(renderer, "../assets/dark_elf.png");
    harpyTexture = IMG_LoadTexture(renderer, "../assets/harpy.png");
    mercenaryTexture = IMG_LoadTexture(renderer, "../assets/mercenary.png");

    if (!ogreTexture || !darkElfTexture || !harpyTexture || !mercenaryTexture) {
        std::cerr << "Error al cargar texturas: " << SDL_GetError() << "\n";
        return false;
    }

    //Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf error: " << TTF_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont("../assets/BigBlueT.ttf", 24);
    if (!font) {
        std::cerr << "SDL_ttf error: " << TTF_GetError() << "\n";
        return false;
    }

    // Initialize WaveManager with the loaded textures
    waveManager = WaveManager(path, ogreTexture, darkElfTexture, harpyTexture, mercenaryTexture);

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

    // Mostrar pantalla de derrota si el jugador pierde
    if (enemiesReachedEnd >= 5) {
        renderDefeatScreen();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
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
                case SDLK_w:
                    // Intentar mejorar la torreta seleccionada
                    for (auto& tower : towers) {
                        if (tower->isMouseOver(mouseX, mouseY)) { // Verificar si el mouse está sobre la torreta
                            if (tower->upgrade(gold)) {
                                std::cout << "Torreta mejorada al nivel " << tower->getLevel() << "\n";
                            } else {
                                std::cout << "No se pudo mejorar la torreta.\n";
                            }
                            break;
                        }
                    }
                    break;
            }
        }
    }
}

void Game::placeTower(int mouseX, int mouseY) {
    int col = mouseX / 75;
    int row = mouseY / 75;

    if (gold < Tower::getCost(selectedTowerType)) {
        // No se puede comprar la torre
        return;
    }

    if (row >= 0 && row < mapSize && col >= 0 && col < mapSize && map[row][col] == 0) {
        // Coloca una torre del tipo seleccionado en la celda
        towers.push_back(std::make_unique<Tower>(col * 75 + 37, row * 75 + 37, selectedTowerType, mapSize));
        map[row][col] = 2; // Marca la celda como ocupada
        gold -= Tower::getCost(selectedTowerType);
    }
}

void Game::update() {
    waveTimer++;
    enemyTimer++;
    
    // Update wave manager every frame to handle enemy spawning
    waveManager.update();
    
    // Get any ready enemies and add them to the game
    auto waveEnemies = waveManager.getCurrentWave();
    if (!waveEnemies.empty()) {
        for (auto& enemy : waveEnemies) {
            if (enemy) {  // Make sure we have a valid enemy
                enemies.push_back(std::move(enemy));
            }
        }
    }
    
    // Process all active enemies using WaveManager
    // This will handle updates, deaths, and enemies that reached the end
    auto [processedEnemies, goldEarned] = waveManager.processActiveEnemies(enemies);
    
    // Add any gold earned to the player's total
    gold += goldEarned;
    
    // Check if a wave transition has occurred and handle it
    bool noEnemiesLeft = enemies.empty();
    waveManager.checkAndHandleWaveTransition(noEnemiesLeft);
    
    // Update towers
    for (auto& tower : towers) {
        tower->update(enemies, projectiles);
    }
    
    // Update projectiles
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        if (!(*it) || (*it)->hasHit()) {  // Check for null pointer
            it = projectiles.erase(it);
        } else {
            (*it)->update();
            ++it;
        }
    }
}

void Game::render() {
    // Renderizar el mapa
    renderMap();

    // Renderizar las torretas
    for (auto& tower : towers) {
        bool isSelected = tower->isMouseOver(mouseX, mouseY); // Verificar si el mouse está sobre la torreta
        tower->render(renderer, isSelected);
    }

    // Renderizar enemigos y proyectiles
    for (auto& enemy : enemies) {
        enemy->render(renderer);
    }
    for (auto& projectile : projectiles) {
        projectile->render(renderer);
    }

    // Renderizar el panel lateral (incluye el oro)
    renderPannel();

    SDL_RenderPresent(renderer);
}

void Game::renderMap() {
    int cellSize = 75; // Tamaño de cada celda (800px / 8 = 100px por celda)
    for (int row = 0; row < mapSize; ++row) {
        for (int col = 0; col < mapSize; ++col) {
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

void Game::renderPannel() {
    // Background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect pannel = {mapSize * 75, 0, 500, mapSize * 75};
    SDL_RenderFillRect(renderer, &pannel);

    // Padding (10)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect pannelRect = {mapSize * 75 + 10, 10, 480, mapSize * 75 - 20};
    SDL_RenderDrawRect(renderer, &pannelRect);

    // Constants for layout management
    const int CONTENT_WIDTH = 420;                  // Width for text wrapping
    const int SECTION_SPACING = 25;                 // Space between sections
    const int ELEMENT_SPACING = 5;                  // Additional space between elements
    const int PANEL_MARGIN_TOP = 20;                // Top margin
    const int PANEL_MARGIN_BOTTOM = 50;             // Bottom margin reserved for controls
    const int MAX_PANEL_HEIGHT = mapSize * 75 - PANEL_MARGIN_BOTTOM;
    
    int yOffset = PANEL_MARGIN_TOP;
    SDL_Color color = {0, 0, 0}; // Black text

    // Helper function to render text at a specific position
    auto renderText = [&](const std::string& text, int& y, SDL_Color textColor = {0, 0, 0}, bool isHeader = false) {
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), textColor, CONTENT_WIDTH);
        if (!textSurface) {
            return 0; // If rendering fails, return 0 height
        }
        
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {mapSize * 75 + 20, y, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        
        // Store the height for return value
        int textHeight = textSurface->h;
        
        // Update yOffset based on actual text height plus some padding
        y += textHeight + (isHeader ? SECTION_SPACING : ELEMENT_SPACING);
        
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        
        return textHeight;
    };
    
    // Check if a section can fit in the remaining space
    auto canFitSection = [&](int sectionHeight) {
        return (yOffset + sectionHeight) < MAX_PANEL_HEIGHT;
    };

    // =========== GAME INFO SECTION ===========
    // Always show game info (essential information)
    std::stringstream ss;
    
    // Section title
    ss.str("");
    ss << "Game Info:";
    renderText(ss.str(), yOffset, color, true);
    
    // Gold Text
    ss.str("");
    ss << "Gold: " << gold;
    renderText(ss.str(), yOffset);
    
    // Wave Text
    ss.str("");
    ss << "Wave: " << waveManager.getCurrentWaveNumber();
    renderText(ss.str(), yOffset);
    
    // Enemy counts
    ss.str("");
    ss << "Enemies Defeated: " << enemiesDefeated;
    renderText(ss.str(), yOffset);
    
    yOffset += SECTION_SPACING / 2; // Half spacing between sections
    
    // =========== EVOLUTION STATS SECTION ===========
    // Get fitness stats
    const FitnessStats& stats = waveManager.getLastWaveFitness();
    
    // Estimate evolution section height to see if it will fit
    int evolutionSectionHeight = 0;
    if (stats.generation > 0) {
        evolutionSectionHeight = 150; // Approximate height of this section based on content
    }
    
    // Only show evolution details if we have at least one generation and space
    if (stats.generation > 0 && canFitSection(evolutionSectionHeight)) {
        // Section title
        ss.str("");
        ss << "Evolution Stats:";
        renderText(ss.str(), yOffset, color, true);
        
        // Generation
        ss.str("");
        ss << "Generation: " << stats.generation;
        renderText(ss.str(), yOffset);
        
        // Mutation Rate
        ss.str("");
        ss << "Mutation: " << std::fixed << std::setprecision(1) << (stats.mutationRate * 100) << "%";
        renderText(ss.str(), yOffset);
        
        // Average Fitness
        ss.str("");
        ss << "Avg Fitness: " << std::fixed << std::setprecision(1) << stats.averageFitness;
        renderText(ss.str(), yOffset);
        
        // Best Fitness
        ss.str("");
        ss << "Best Fitness: " << std::fixed << std::setprecision(1) << stats.maxFitness;
        renderText(ss.str(), yOffset);
        
        yOffset += SECTION_SPACING / 2; // Half spacing between sections
    }
    
    // =========== AVAILABLE TOWERS SECTION ===========
    // Estimate towers section height to see if it will fit
    int towersSectionHeight = 140; // Approximate height based on content
    
    // Only show if there's still space
    if (canFitSection(towersSectionHeight)) {
        // Section title
        ss.str("");
        ss << "Build Towers:";
        renderText(ss.str(), yOffset, color, true);
        
        // Archer tower
        ss.str("");
        ss << "1: Archer (" << Tower::getCost(TowerType::Archer) << " gold)";
        SDL_Color archerColor = (selectedTowerType == TowerType::Archer) ? SDL_Color{0, 128, 0} : color;
        renderText(ss.str(), yOffset, archerColor);
        
        // Mage tower
        ss.str("");
        ss << "2: Mage (" << Tower::getCost(TowerType::Mage) << " gold)";
        SDL_Color mageColor = (selectedTowerType == TowerType::Mage) ? SDL_Color{0, 0, 128} : color;
        renderText(ss.str(), yOffset, mageColor);
        
        // Artillery tower
        ss.str("");
        ss << "3: Artillery (" << Tower::getCost(TowerType::Artillery) << " gold)";
        SDL_Color artilleryColor = (selectedTowerType == TowerType::Artillery) ? SDL_Color{128, 64, 0} : color;
        renderText(ss.str(), yOffset, artilleryColor);
        
        yOffset += SECTION_SPACING / 2; // Half spacing between sections
    }
    
    // =========== TOWER INFO SECTION ===========
    // Only check for tower info if there's enough space left
    if (yOffset < MAX_PANEL_HEIGHT - 100) {
        // Show tower info when hovering over one
        for (auto& tower : towers) {
            if (tower->isMouseOver(mouseX, mouseY)) {
                // Estimate tower info section height
                int towerInfoHeight = 120; // Approximate height based on content
                
                if (canFitSection(towerInfoHeight)) {
                    // Section title
                    ss.str("");
                    ss << "Tower Info:";
                    renderText(ss.str(), yOffset, color, true);
                    
                    // Tower type
                    ss.str("");
                    ss << "Type: " << (tower->getType() == TowerType::Archer ? "Archer" :
                                      tower->getType() == TowerType::Mage ? "Mage" : "Artillery");
                    renderText(ss.str(), yOffset);
                    
                    // Tower level
                    ss.str("");
                    ss << "Level: " << tower->getLevel();
                    renderText(ss.str(), yOffset);
                    
                    // Upgrade info
                    ss.str("");
                    int upgradeCost = Tower::getCost(tower->getType()) * (tower->getLevel() + 1);
                    ss << "Upgrade Cost: " << upgradeCost << " gold";
                    if (tower->getLevel() >= 4) {
                        ss << " (Max Level)";
                    } else if (gold < upgradeCost) {
                        ss << " (Need more gold)";
                    } else {
                        ss << " (Press 'W' to upgrade)";
                    }
                    
                    renderText(ss.str(), yOffset);
                }
                break; // Only show info for one tower at a time
            }
        }
    }
    
    // =========== CONTROLS SECTION ===========
    // Always show controls at the bottom
    ss.str("");
    ss << "Controls: 1-3 select tower, W upgrades";
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, ss.str().c_str(), color, CONTENT_WIDTH);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {mapSize * 75 + 20, mapSize * 75 - textSurface->h - 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::renderDefeatScreen() {
    // Limpiar la pantalla
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Negro
    SDL_RenderClear(renderer);

    // Mostrar mensaje de derrota
    SDL_Color color = {255, 0, 0}; // Rojo
    std::string message = "You Lost! Press ESC to Exit.";
    SDL_Surface* surface = TTF_RenderText_Blended(font, message.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {mapSize * 75 / 2 - surface->w / 2, mapSize * 75 / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);

    // Esperar a que el jugador presione ESC
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
                return;
            }
        }
    }
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
