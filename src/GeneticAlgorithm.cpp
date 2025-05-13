#include "GeneticAlgorithm.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>
#include "Enemy.h"

GeneticAlgorithm::GeneticAlgorithm() {}

std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::evolvePopulation(const std::vector<std::unique_ptr<Enemy>>& previousWave) {
    std::vector<std::unique_ptr<Enemy>> newWave;

    // Select 4 parents
    std::vector<std::unique_ptr<Enemy>> parents = selectParents(previousWave);
    
    // If we don't have enough parents, return the previous wave
    if (parents.size() < 4) {
        std::cout << "Not enough parents to evolve population, returning previous wave" << std::endl;
        
        // Clone the previous wave
        for (const auto& enemy : previousWave) {
            newWave.push_back(std::unique_ptr<Enemy>(enemy->clone()));
        }
        return newWave;
    }

    // Create 2 pairs of parents and perform crossover to get 4 children
    std::vector<std::unique_ptr<Enemy>> offspring1 = crossover(parents[0], parents[1]);
    std::vector<std::unique_ptr<Enemy>> offspring2 = crossover(parents[2], parents[3]);
    
    // Combine all offspring
    std::vector<std::unique_ptr<Enemy>> allOffspring;
    for (auto& child : offspring1) {
        allOffspring.push_back(std::move(child));
    }
    for (auto& child : offspring2) {
        allOffspring.push_back(std::move(child));
    }

    // Create new generation with 4 parents and 4 children
    newWave = createNextGeneration(parents, allOffspring);

    return newWave;
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
    
    // Take the top 4 parents
    std::vector<std::unique_ptr<Enemy>> parents;
    int numParents = std::min(4, static_cast<int>(indices.size()));
    
    for (int i = 0; i < numParents; ++i) {
        parents.push_back(std::unique_ptr<Enemy>(population[indices[i]]->clone()));
    }
    
    return parents;
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

void GeneticAlgorithm::mutate(std::unique_ptr<Enemy>& enemy, float mutationRate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    if (dis(gen) < mutationRate) {
        enemy->mutate(0.01f);
    }
}


std::vector<std::unique_ptr<Enemy>> GeneticAlgorithm::createNextGeneration(
    const std::vector<std::unique_ptr<Enemy>>& parents,
    const std::vector<std::unique_ptr<Enemy>>& offspring) {
    
    std::vector<std::unique_ptr<Enemy>> newGeneration;
    newGeneration.reserve(parents.size() + offspring.size());
    
    // Create copies of parents using clone()
    for (const auto& parent : parents) {
        newGeneration.push_back(std::unique_ptr<Enemy>(parent->clone()));
    }
    
    // Create copies of offspring using clone()
    for (const auto& child : offspring) {
        newGeneration.push_back(std::unique_ptr<Enemy>(child->clone()));
    }
    
    return newGeneration;
}