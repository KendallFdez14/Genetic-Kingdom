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
        std::cout << "Wave " << currentWaveNumber << " complete" << std::endl;

        
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
    // Create 3 enemies of each type, for a total of 12 enemies
    std::vector<std::unique_ptr<Enemy>> initialWave;
    for (int i = 0; i < 3; i++) {
        initialWave.push_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
        initialWave.push_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
        initialWave.push_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
        initialWave.push_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
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
                Enemy* enemyCopy = enemy->clone();
                
                this->survivors.push_back(std::unique_ptr<Enemy>(enemyCopy));
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
        this->survivors.push_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
        this->survivors.push_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
        this->survivors.push_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
        this->survivors.push_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
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

void WaveManager::calculateFitnessStats() {
    if (processedEnemies.empty()) {
        return; // No enemies to calculate stats for
    }
    
    float totalFitness = 0.0f;
    float maxFitness = 0.0f;
    float minFitness = std::numeric_limits<float>::max();
    
    // Calculate fitness statistics
    for (const auto& enemy : processedEnemies) {
        float fitness = enemy->calculateFitness();
        totalFitness += fitness;
        
        if (fitness > maxFitness) {
            maxFitness = fitness;
        }
        
        if (fitness < minFitness) {
            minFitness = fitness;
        }
    }
    
    // Update fitness stats
    lastWaveFitness.averageFitness = totalFitness / processedEnemies.size();
    lastWaveFitness.maxFitness = maxFitness;
    lastWaveFitness.minFitness = minFitness;
    lastWaveFitness.generation = geneticAlgorithm.getGeneration();
    lastWaveFitness.mutationRate = geneticAlgorithm.getMutationRate();
    
    std::cout << "Wave " << currentWaveNumber << " Fitness Stats: " 
              << "Avg: " << lastWaveFitness.averageFitness
              << ", Max: " << lastWaveFitness.maxFitness
              << ", Min: " << lastWaveFitness.minFitness
              << ", Gen: " << lastWaveFitness.generation
              << ", Mutation: " << lastWaveFitness.mutationRate * 100.0f << "%" << std::endl;
}

void WaveManager::prepareNextWave() {
    // Calculate fitness statistics for the processed enemies before preparing next wave
    calculateFitnessStats();
    
    // Combine processed enemies with any remaining enemies
    std::cout << "Preparing next wave with " << processedEnemies.size() << " processed enemies" << std::endl;
    
    // If we don't have enough processed enemies, create default ones
    // We need at least 12 enemies total (6 parents to generate 6 children)
    if (processedEnemies.size() < 12) {
        std::cout << "Not enough processed enemies, adding default ones to reach 12" << std::endl;
        
        // Calculate how many more enemies we need
        int neededEnemies = 12 - processedEnemies.size();
        
        // Add enemies of each type in a balanced way
        int eachTypeCount = neededEnemies / 4;
        int remainder = neededEnemies % 4;
        
        for (int i = 0; i < eachTypeCount; i++) {
            processedEnemies.push_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
            processedEnemies.push_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
            processedEnemies.push_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
            processedEnemies.push_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
        }
        
        // Add any remainder enemies
        for (int i = 0; i < remainder; i++) {
            switch (i % 4) {
                case 0:
                    processedEnemies.push_back(std::make_unique<Ogre>(0, 0, path, ogreTexture));
                    break;
                case 1:
                    processedEnemies.push_back(std::make_unique<DarkElf>(0, 0, path, darkElfTexture));
                    break;
                case 2:
                    processedEnemies.push_back(std::make_unique<Harpy>(0, 0, path, harpyTexture));
                    break;
                case 3:
                    processedEnemies.push_back(std::make_unique<Mercenary>(0, 0, path, mercenaryTexture));
                    break;
            }
        }
    }
    
    // Save processed enemies as survivors for the genetic algorithm
    survivors = std::move(processedEnemies);
    processedEnemies.clear();
    
    std::cout << "Next wave will evolve from " << survivors.size() << " enemies" << std::endl;
}

std::pair<std::vector<std::unique_ptr<Enemy>>, int> WaveManager::processActiveEnemies(std::vector<std::unique_ptr<Enemy>>& activeEnemies) {
    // Vector to store enemies that died or reached the end this frame
    std::vector<std::unique_ptr<Enemy>> processedThisFrame;
    int goldEarned = 0;
    
    // Update and process enemies
    for (auto it = activeEnemies.begin(); it != activeEnemies.end();) {
        if (!(*it)) {  // Check for null pointer
            it = activeEnemies.erase(it);
        } else if ((*it)->isDead() || (*it)->hasReachedEnd()) {
            // Enemy died or reached end, save it for genetic algorithm
            if (*it) {
                // Add gold if enemy died
                if ((*it)->isDead()) {
                    goldEarned += (*it)->getGold();
                }
                
                // Move to processed enemies list for genetic algorithm
                processedThisFrame.push_back(std::move(*it));
            }
            it = activeEnemies.erase(it);
        } else {
            (*it)->update();
            ++it;
        }
    }
    
    // Add processed enemies to the WaveManager's list
    if (!processedThisFrame.empty()) {
        addProcessedEnemies(std::move(processedThisFrame));
    }
    
    // Return a copy of processed enemies and gold earned
    return {std::move(processedThisFrame), goldEarned};
}

bool WaveManager::checkAndHandleWaveTransition(bool noEnemiesLeft) {
    bool transitionOccurred = false;
    
    // If wave just ended and all enemies are gone, save any remaining enemies
    if ((wasWaveActive && !waveActive) || (!waveActive && noEnemiesLeft)) {
        // Trigger new wave planning if appropriate
        prepareNextWave();
        transitionOccurred = true;
    }
    
    // Update previous wave state
    wasWaveActive = waveActive;
    
    return transitionOccurred;
}
