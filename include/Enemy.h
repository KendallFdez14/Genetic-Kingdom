#pragma once

#include <vector>
#include <memory>
#include <string>
#include <SDL2/SDL.h>

class Enemy {
protected:
    int x, y;
    int health;
    int initialHealth; // Store initial health for fitness calculation
    float speed;
    float arrowResistance;
    float magicResistance;
    float artilleryResistance;
    int gold;
    int currentNode = 0;
    std::vector<std::pair<int, int>> path;
    SDL_Texture* texture;
    SDL_Rect destRect;
    
    // Fitness calculation fields
    int hitsTaken = 0;
    float timeTaken = 0;
    float distanceTraveled = 0;
    float pathLength = 0;

<<<<<<< HEAD
public:
    Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, 
          int health, float speed, float arrowRes, float magicRes, 
          float artilleryRes, SDL_Texture* texture, int gold);
    
    virtual ~Enemy() = default;
    
    virtual void update();
    virtual void render(SDL_Renderer* renderer);
    
    // Getters
    int getX() const;
    int getY() const;
    int getHealth() const { return health; }
    int getInitialHealth() const { return initialHealth; } // Get initial health
    float getSpeed() const { return speed; }
    int getDamage() const { return 1; }
    int getGold() const;
    std::vector<std::pair<int, int>> getPath() const;
    SDL_Texture* getTexture() const;
    
    // Resistance getters
    float getArrowResistance() const { return arrowResistance; }
    float getMagicResistance() const { return magicResistance; }
    float getArtilleryResistance() const { return artilleryResistance; }
    
    // Combat methods
    void takeDamage(int dmg, const std::string& type);
    
    // Status methods
    bool isDead() const;
    bool hasReachedEnd() const { return currentNode >= path.size() - 1; }
    
    // Genetic algorithm methods
    Enemy* clone() const;
    void mutate(float mutationRate);
    float calculateFitness() const;
    
    // Setters for genetic algorithm
    void setHealth(int h) { health = h; initialHealth = h; } // Update both values
    void setSpeed(float s) { speed = s; }
    void setArrowResistance(float r) { arrowResistance = r; }
    void setMagicResistance(float r) { magicResistance = r; }
    void setArtilleryResistance(float r) { artilleryResistance = r; }
    void setGold(int g) { gold = g; }
    
    // Static method for crossover
    static Enemy* crossover(const Enemy* parent1, const Enemy* parent2,
                           const std::vector<std::pair<int, int>>& path,
                           SDL_Texture* texture);
=======
        void update();
        void render(SDL_Renderer* renderer);
        void takeDamage(int damage, const std::string& type);
        bool isDead() const;
        int getX() const;
        int getY() const;
        int getGold() const;
        bool hasReachedEnd() const; // Verificar si el enemigo llegó al final del camino
    protected:
        int x, y;
        int health;
        int gold;
        float speed;

        float arrowResistance;
        float magicResistance;
        float artilleryResistance;

        std::vector<std::pair<int, int>> path;
        size_t currentNode = 0;

        SDL_Texture* texture; // Textura del enemigo
>>>>>>> Develop
};

// Enemy types
class Ogre : public Enemy {
public:
    Ogre(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

class DarkElf : public Enemy {
public:
    DarkElf(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

class Harpy : public Enemy {
public:
    Harpy(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

class Mercenary : public Enemy {
public:
    Mercenary(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
}; 