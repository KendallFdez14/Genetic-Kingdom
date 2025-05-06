#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Enemy {
    public:
        Enemy(int x, int y, const std::vector<std::pair<int, int>>& path, int health, float speed, float arrowRes, float magicRes, float artilleryRes, SDL_Texture* texture);
        virtual ~Enemy() = default;

        void update();
        void render(SDL_Renderer* renderer);
        void takeDamage(int dmg, const std::string& type);
        bool isDead() const;
        int getX() const;
        int getY() const;

    protected:
        int x, y;
        float speed;
        int health;

        float arrowResistance;
        float magicResistance;
        float artilleryResistance;

        std::vector<std::pair<int, int>> path;
        size_t currentNode = 0;

        SDL_Texture* texture; // Textura del enemigo
};

// Ogro
class Ogre : public Enemy {
    public:
        Ogre(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

// Elfo Oscuro
class DarkElf : public Enemy {
    public:
        DarkElf(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

// Harpía
class Harpy : public Enemy {
    public:
        Harpy(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};

// Mercenario
class Mercenary : public Enemy {
    public:
        Mercenary(int x, int y, const std::vector<std::pair<int, int>>& path, SDL_Texture* texture);
};