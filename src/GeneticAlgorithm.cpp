#include "GeneticAlgorithm.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>
#include "Enemy.h"

GeneticAlgorithm::GeneticAlgorithm() {}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::evolvePopulation(const std::vector<std::unique_ptr<Enemy>>& previousWave) {
    std::vector<std::unique_ptr<Enemy>> newWave;
    
    // Track generations
    generation++;
    
    // Update mutation rate based on progress
    updateMutationRate(previousWave);
    
    std::cout << "Generation " << generation << ", adaptive mutation rate: " << adaptiveMutationRate << std::endl;

    // Select 6 parents (increased from 4)
    std::vector<std::unique_ptr<Enemy>> parents = selectParents(previousWave);
    
    // If we don't have enough parents, return the previous wave
    if (parents.size() < 6) {
        std::cout << "Not enough parents to evolve population, returning previous wave" << std::endl;
        
        // Clone the previous wave
        for (const auto& enemy : previousWave) {
            newWave.push_back(std::unique_ptr<Enemy>(enemy->clone()));
        }
        return newWave;
    }

    // Create 3 pairs of parents and perform crossover to get 6 children
    std::vector<std::unique_ptr<Enemy>> offspring1 = crossover(parents[0], parents[1]);
    std::vector<std::unique_ptr<Enemy>> offspring2 = crossover(parents[2], parents[3]);
    std::vector<std::unique_ptr<Enemy>> offspring3 = crossover(parents[4], parents[5]);
    
    // Combine all offspring
    std::vector<std::unique_ptr<Enemy>> allOffspring;
    for (auto& child : offspring1) {
        allOffspring.push_back(std::move(child));
    }
    for (auto& child : offspring2) {
        allOffspring.push_back(std::move(child));
    }
    for (auto& child : offspring3) {
        allOffspring.push_back(std::move(child));
    }

    // Create new generation with 6 parents and 6 children for a total of 12
    newWave = createNextGeneration(parents, allOffspring);
    
    // Apply immigration every few generations
    if (generation % immigrationInterval == 0) {
        // Extract path and textures from the first enemy
        std::vector<std::pair<int, int>> path;
        SDL_Texture* ogreTexture = nullptr;
        SDL_Texture* darkElfTexture = nullptr;
        SDL_Texture* harpyTexture = nullptr;
        SDL_Texture* mercenaryTexture = nullptr;
        
        // Find reference textures
        for (const auto& enemy : newWave) {
            path = enemy->getPath();
            if (dynamic_cast<Ogre*>(enemy.get())) {
                ogreTexture = enemy->getTexture();
            } else if (dynamic_cast<DarkElf*>(enemy.get())) {
                darkElfTexture = enemy->getTexture();
            } else if (dynamic_cast<Harpy*>(enemy.get())) {
                harpyTexture = enemy->getTexture();
            } else if (dynamic_cast<Mercenary*>(enemy.get())) {
                mercenaryTexture = enemy->getTexture();
            }
        }
        
        // Create random immigrants if we have all textures
        if (ogreTexture && darkElfTexture && harpyTexture && mercenaryTexture) {
            int immigrantCount = static_cast<int>(newWave.size() * immigrationRate);
            std::cout << "Introducing " << immigrantCount << " new random enemies for genetic diversity" << std::endl;
            
            // Create random enemies
            auto immigrants = createRandomEnemies(immigrantCount, path, ogreTexture, darkElfTexture, harpyTexture, mercenaryTexture);
            
            // Remove weakest members from population
            std::vector<size_t> indices(newWave.size());
            std::iota(indices.begin(), indices.end(), 0);
            std::sort(indices.begin(), indices.end(),
                [&newWave](size_t i1, size_t i2) {
                    return newWave[i1]->calculateFitness() < newWave[i2]->calculateFitness();
                });
            
            // Remove weakest members (ones with lowest fitness)
            std::vector<std::unique_ptr<Enemy>> tempWave;
            for (size_t i = immigrantCount; i < newWave.size(); i++) {
                tempWave.push_back(std::move(newWave[indices[i]]));
            }
            newWave = std::move(tempWave);
            
            // Add immigrants
            for (auto& immigrant : immigrants) {
                newWave.push_back(std::move(immigrant));
            }
        }
    }

    return newWave;
}

void GeneticAlgorithm::updateMutationRate(const std::vector<std::unique_ptr<Enemy>>& population) {
    // Find best fitness
    float bestFitness = 0.0f;
    for (const auto& enemy : population) {
        float fitness = enemy->calculateFitness();
        if (fitness > bestFitness) bestFitness = fitness;
    }
    
    // First generation doesn't have a previous fitness to compare
    if (generation == 1) {
        previousBestFitness = bestFitness;
        return;
    }
    
    std::cout << "Best fitness: " << bestFitness << " (previous: " << previousBestFitness << ")" << std::endl;
    
    // Check for stagnation (less than 1% improvement)
    if (bestFitness <= previousBestFitness * 1.01f) {
        stagnantGenerations++;
        std::cout << "Stagnant generation detected (" << stagnantGenerations << " in a row)" << std::endl;
        
        if (stagnantGenerations > 2) {
            // Increase mutation rate to escape local optimum
            adaptiveMutationRate = std::min(adaptiveMutationRate * 1.5f, 0.3f);
            std::cout << "Increasing mutation rate to " << adaptiveMutationRate << " to escape local optimum" << std::endl;
        }
    } else {
        // Reset stagnation counter and mutation rate when improving
        stagnantGenerations = 0;
        adaptiveMutationRate = baseMutationRate;
        std::cout << "Progress detected, resetting mutation rate to base rate: " << baseMutationRate << std::endl;
    }
    
    previousBestFitness = bestFitness;
}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::createRandomEnemies(int count, const std::vector<std::pair<int, int>>& path, SDL_Texture* ogreTexture, SDL_Texture* darkElfTexture, SDL_Texture* harpyTexture, SDL_Texture* mercenaryTexture) {
    std::vector<std::unique_ptr<Enemy>> randomEnemies;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> typeDist(0, 3);
    std::uniform_real_distribution<float> speedDist(1.0f, 4.0f);
    std::uniform_int_distribution<> healthDist(80, 200);
    std::uniform_real_distribution<float> resistanceDist(0.0f, 0.7f);
    std::uniform_int_distribution<> goldDist(8, 20);
    
    for (int i = 0; i < count; i++) {
        std::unique_ptr<Enemy> enemy;
        int type = typeDist(gen);
        
        // Create a random enemy of a random type
        switch (type) {
            case 0:
                enemy = std::make_unique<Ogre>(0, 0, path, ogreTexture);
                break;
            case 1:
                enemy = std::make_unique<DarkElf>(0, 0, path, darkElfTexture);
                break;
            case 2:
                enemy = std::make_unique<Harpy>(0, 0, path, harpyTexture);
                break;
            case 3:
                enemy = std::make_unique<Mercenary>(0, 0, path, mercenaryTexture);
                break;
        }
        
        // Set random attributes
        enemy->setHealth(healthDist(gen));
        enemy->setSpeed(speedDist(gen));
        enemy->setArrowResistance(resistanceDist(gen));
        enemy->setMagicResistance(resistanceDist(gen));
        enemy->setArtilleryResistance(resistanceDist(gen));
        enemy->setGold(goldDist(gen));
        
        randomEnemies.push_back(std::move(enemy));
    }
    
    return randomEnemies;
}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::selectParents(const std::vector<std::unique_ptr<Enemy>>& population) {
    // Create a vector of indices to sort
    std::vector<size_t> indices(population.size());
    std::iota(indices.begin(), indices.end(), 0);
    
    // Sort indices based on fitness
    std::sort(indices.begin(), indices.end(),
        [&population](size_t i1, size_t i2) {
            return population[i1]->calculateFitness() > population[i2]->calculateFitness();
        });
    
    // Take the top 6 parents (increased from 4)
    std::vector<std::unique_ptr<Enemy>> parents;
    int numParents = std::min(6, static_cast<int>(indices.size()));
    
    for (int i = 0; i < numParents; ++i) {
        parents.push_back(std::unique_ptr<Enemy>(population[indices[i]]->clone()));
    }
    
    return parents;
}

void GeneticAlgorithm::mutate(std::unique_ptr<Enemy>& enemy, float mutationRate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Apply mutation with adaptive rate
    if (dis(gen) < adaptiveMutationRate) {
        // Use more aggressive mutation
        enemy->mutate(adaptiveMutationRate);
    }
}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::crossover(const std::unique_ptr<Enemy>& parent1, const std::unique_ptr<Enemy>& parent2) {
    std::vector<std::pair<int, int>> path = parent1->getPath();
    SDL_Texture* texture1 = parent1->getTexture();
    SDL_Texture* texture2 = parent2->getTexture();

    // Posición inicial en el camino
    int startX = path[0].first * 75 + 37;
    int startY = path[0].second * 75 + 37;
    
    // Variables temporales para los genes
    int health1, health2;
    float speed1, speed2;
    float arrowRes1, arrowRes2;
    float magicRes1, magicRes2;
    float artilleryRes1, artilleryRes2;
    int gold1, gold2;
    
    // Crossover de un punto
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> crossoverPoint(0, 5); // 6 "genes" en total
    
    int point = crossoverPoint(gen);
    
    // Aplicamos crossover según el punto aleatorio
    // 0-health, 1-speed, 2-arrowRes, 3-magicRes, 4-artilleryRes, 5-gold
    
    // Para el primer hijo
    health1 = (point >= 0) ? parent2->getHealth() : parent1->getHealth();
    speed1 = (point >= 1) ? parent2->getSpeed() : parent1->getSpeed();
    arrowRes1 = (point >= 2) ? parent2->getArrowResistance() : parent1->getArrowResistance();
    magicRes1 = (point >= 3) ? parent2->getMagicResistance() : parent1->getMagicResistance();
    artilleryRes1 = (point >= 4) ? parent2->getArtilleryResistance() : parent1->getArtilleryResistance();
    gold1 = (point >= 5) ? parent2->getGold() : parent1->getGold();
    
    // Para el segundo hijo (invertimos los padres)
    health2 = (point >= 0) ? parent1->getHealth() : parent2->getHealth();
    speed2 = (point >= 1) ? parent1->getSpeed() : parent2->getSpeed();
    arrowRes2 = (point >= 2) ? parent1->getArrowResistance() : parent2->getArrowResistance();
    magicRes2 = (point >= 3) ? parent1->getMagicResistance() : parent2->getMagicResistance();
    artilleryRes2 = (point >= 4) ? parent1->getArtilleryResistance() : parent2->getArtilleryResistance();
    gold2 = (point >= 5) ? parent1->getGold() : parent2->getGold();
    
    // Create the two new enemies using concrete types based on the parents
    std::vector<std::unique_ptr<Enemy>> children;
    
    // First child based on parent1's type
    std::unique_ptr<Enemy> child1;
    if (dynamic_cast<const Ogre*>(parent1.get())) {
        child1 = std::make_unique<Ogre>(startX, startY, path, texture1);
    } else if (dynamic_cast<const DarkElf*>(parent1.get())) {
        child1 = std::make_unique<DarkElf>(startX, startY, path, texture1);
    } else if (dynamic_cast<const Harpy*>(parent1.get())) {
        child1 = std::make_unique<Harpy>(startX, startY, path, texture1);
    } else if (dynamic_cast<const Mercenary*>(parent1.get())) {
        child1 = std::make_unique<Mercenary>(startX, startY, path, texture1);
    } else {
        child1 = std::make_unique<Ogre>(startX, startY, path, texture1);
    }
    
    // Second child based on parent2's type
    std::unique_ptr<Enemy> child2;
    if (dynamic_cast<const Ogre*>(parent2.get())) {
        child2 = std::make_unique<Ogre>(startX, startY, path, texture2);
    } else if (dynamic_cast<const DarkElf*>(parent2.get())) {
        child2 = std::make_unique<DarkElf>(startX, startY, path, texture2);
    } else if (dynamic_cast<const Harpy*>(parent2.get())) {
        child2 = std::make_unique<Harpy>(startX, startY, path, texture2);
    } else if (dynamic_cast<const Mercenary*>(parent2.get())) {
        child2 = std::make_unique<Mercenary>(startX, startY, path, texture2);
    } else {
        child2 = std::make_unique<Ogre>(startX, startY, path, texture2);
    }
    
    // Set attributes for the first child
    child1->setHealth(health1);
    child1->setSpeed(speed1);
    child1->setArrowResistance(arrowRes1);
    child1->setMagicResistance(magicRes1);
    child1->setArtilleryResistance(artilleryRes1);
    child1->setGold(gold1);
    
    // Set attributes for the second child
    child2->setHealth(health2);
    child2->setSpeed(speed2);
    child2->setArrowResistance(arrowRes2);
    child2->setMagicResistance(magicRes2);
    child2->setArtilleryResistance(artilleryRes2);
    child2->setGold(gold2);
    
    children.push_back(std::move(child1));
    children.push_back(std::move(child2));
    
    return children;
}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::createNextGeneration(
    const std::vector<std::unique_ptr<Enemy>>& parents,
    const std::vector<std::unique_ptr<Enemy>>& offspring) {
    
    std::vector<std::unique_ptr<Enemy>> newGeneration;
    // Reserve more space for increased population size (12)
    newGeneration.reserve(parents.size() + offspring.size());
    
    // Create copies of parents using clone()
    for (const auto& parent : parents) {
        // Apply mutation to parents as well
        auto clone = std::unique_ptr<Enemy>(parent->clone());
        mutate(clone, adaptiveMutationRate);
        newGeneration.push_back(std::move(clone));
    }
    
    // Create copies of offspring using clone() and apply mutation
    for (const auto& child : offspring) {
        auto clone = std::unique_ptr<Enemy>(child->clone());
        mutate(clone, adaptiveMutationRate * 1.5f); // Slightly higher mutation for offspring
        newGeneration.push_back(std::move(clone));
    }
    
    return newGeneration;
}