#pragma once
#include <SDL2/SDL.h>
#include <vector>

class Enemy {
    public:
        Enemy(int x, int y, const std::vector<std::pair<int, int>>& path);

        void update();
        void render(SDL_Renderer* renderer);
        void takeDamage(int dmg);
        bool isDead() const;
        int getX() const;
        int getY() const;

    private:
        int x, y;
        float speed = 2.0f; // Aumenta la velocidad para que el movimiento sea más visible
        int health = 100;

        std::vector<std::pair<int, int>> path; // Lista de nodos del camino
        size_t currentNode = 0; // Nodo actual que el enemigo está siguiendo
};