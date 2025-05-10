#include "WaveManager.h"
#include "Enemy.h"
#include "GeneticAlgorithm.h"
#include <iostream>
#include <vector>
#include <memory>

WaveManager::WaveManager(const std::vector<std::pair<int, int>>& path, SDL_Texture* ogreTexture, SDL_Texture* darkElfTexture, SDL_Texture* harpyTexture, SDL_Texture* mercenaryTexture)
    : path(path), ogreTexture(ogreTexture), darkElfTexture(darkElfTexture), harpyTexture(harpyTexture), mercenaryTexture(mercenaryTexture), currentWaveNumber(0) {}

void WaveManager::update() {
    //Si la oleada actual no esta activa y no hay ninguna oleada en curso, se crea una nueva oleada
    if (!waveActive && waveTimer >= 1200) {
        startNewWave();
        spawnTimer = 0;
        waveTimer = 0;
        return;  // Don't process anything else this frame
    }
    
    // Update wave timer
    waveTimer++;
    
    // Update spawn timer
    spawnTimer++;
    
    // If it's time to spawn the next enemy and we have enemies left
    if (spawnTimer >= 60 && !currentWave.empty()) { // 60 frames = 1 second at 60 FPS
        // Move the first enemy to the ready queue
        if (!currentWave.empty()) {
            auto enemy = std::move(currentWave.front());
            if (enemy) {  // Make sure we have a valid enemy
                readyEnemies.push_back(std::move(enemy));
                currentWave.erase(currentWave.begin());
            }
        }
        spawnTimer = 0;
    }
    
    // If we've spawned all enemies and there are no more ready enemies, mark wave as complete
    if (currentWave.empty() && readyEnemies.empty()) {
        waveActive = false;
        //std::cout << "Wave " << currentWaveNumber << " complete" << std::endl;
    }
}

void WaveManager::startNewWave() {
    // Clear any remaining enemies from previous wave
    currentWave.clear();
    readyEnemies.clear();
    
    if (currentWaveNumber == 0) {
        //Generar oleada inicial
        currentWave = generateInitialWave();
    } else {
        //Generar nueva oleada
        std::vector<std::unique_ptr<Enemy>> newWave = geneticAlgorithm.evolvePopulation(survivors);
        currentWave = std::move(newWave);
    }
    
    waveActive = true;
    currentWaveNumber++;
    std::cout << "Starting wave " << currentWaveNumber << " with " << currentWave.size() << " enemies" << std::endl;
}

std::vector<std::unique_ptr<Enemy>> WaveManager::generateInitialWave() {
    //2 enemigos por tipo
    std::vector<std::unique_ptr<Enemy>> initialWave;
    for (int i = 0; i < 2; i++) {
        initialWave.push_back(std::make_unique<Ogre>(path[0].first, path[0].second, path, ogreTexture));
        initialWave.push_back(std::make_unique<DarkElf>(path[0].first, path[0].second, path, darkElfTexture));
        initialWave.push_back(std::make_unique<Harpy>(path[0].first, path[0].second, path, harpyTexture));
        initialWave.push_back(std::make_unique<Mercenary>(path[0].first, path[0].second, path, mercenaryTexture));
    }
    return initialWave;
}

void WaveManager::saveSurvivors(const std::vector<std::unique_ptr<Enemy>>& survivors) {
    //Guardar supervivientes para la siguiente oleada
    this->survivors.clear();
    
    // Print debug information
    std::cout << "Saving survivors: " << survivors.size() << " enemies" << std::endl;
    
    // Mover los supervivientes uno a uno
    for (const auto& enemy : survivors) {
        if (enemy && !enemy->isDead()) {
            try {
                // Crear una copia del enemigo
                // Determinar qué tipo de enemigo es
                if (dynamic_cast<Ogre*>(enemy.get())) {
                    this->survivors.push_back(std::make_unique<Ogre>(
                        path[0].first, path[0].second, path, ogreTexture));
                } else if (dynamic_cast<DarkElf*>(enemy.get())) {
                    this->survivors.push_back(std::make_unique<DarkElf>(
                        path[0].first, path[0].second, path, darkElfTexture));
                } else if (dynamic_cast<Harpy*>(enemy.get())) {
                    this->survivors.push_back(std::make_unique<Harpy>(
                        path[0].first, path[0].second, path, harpyTexture));
                } else if (dynamic_cast<Mercenary*>(enemy.get())) {
                    this->survivors.push_back(std::make_unique<Mercenary>(
                        path[0].first, path[0].second, path, mercenaryTexture));
                }
            } catch (const std::exception& e) {
                std::cerr << "Error saving survivor: " << e.what() << std::endl;
            }
        }
    }
    
    std::cout << "Saved " << this->survivors.size() << " survivors" << std::endl;
    
    // If no survivors, create at least some initial enemies
    if (this->survivors.empty()) {
        std::cout << "No survivors, creating default enemies for next wave" << std::endl;
        // Add at least one of each type to ensure we have some enemies
        this->survivors.push_back(std::make_unique<Ogre>(path[0].first, path[0].second, path, ogreTexture));
        this->survivors.push_back(std::make_unique<DarkElf>(path[0].first, path[0].second, path, darkElfTexture));
        this->survivors.push_back(std::make_unique<Harpy>(path[0].first, path[0].second, path, harpyTexture));
        this->survivors.push_back(std::make_unique<Mercenary>(path[0].first, path[0].second, path, mercenaryTexture));
    }
}

std::vector<std::unique_ptr<Enemy>> WaveManager::getCurrentWave() {
    if (readyEnemies.empty()) {
        return std::vector<std::unique_ptr<Enemy>>();
    }
    
    std::vector<std::unique_ptr<Enemy>> waveCopy;
    waveCopy = std::move(readyEnemies);
    readyEnemies.clear();
    
    return waveCopy;
}

bool WaveManager::isWaveComplete() {
    return currentWave.empty();
}

int WaveManager::getCurrentWaveNumber() {
    return currentWaveNumber;
}

bool WaveManager::isWaveActive() {
    return waveActive;
}

void WaveManager::addProcessedEnemies(std::vector<std::unique_ptr<Enemy>> deadEnemies) {
    // Add dead or finished enemies to the processed list
    for (auto& enemy : deadEnemies) {
        if (enemy) {
            processedEnemies.push_back(std::move(enemy));
        }
    }
}

void WaveManager::prepareNextWave() {
    // Combine processed enemies with any remaining enemies
    std::cout << "Preparing next wave with " << processedEnemies.size() << " processed enemies" << std::endl;
    
    // If we don't have enough processed enemies, create default ones
    if (processedEnemies.size() < 4) {
        std::cout << "Not enough processed enemies, adding default ones" << std::endl;
        // Add at least one of each type to ensure we have some enemies
        processedEnemies.push_back(std::make_unique<Ogre>(path[0].first, path[0].second, path, ogreTexture));
        processedEnemies.push_back(std::make_unique<DarkElf>(path[0].first, path[0].second, path, darkElfTexture));
        processedEnemies.push_back(std::make_unique<Harpy>(path[0].first, path[0].second, path, harpyTexture));
        processedEnemies.push_back(std::make_unique<Mercenary>(path[0].first, path[0].second, path, mercenaryTexture));
    }
    
    // Save processed enemies as survivors for the genetic algorithm
    survivors = std::move(processedEnemies);
    processedEnemies.clear();
    
    std::cout << "Next wave will evolve from " << survivors.size() << " enemies" << std::endl;
}