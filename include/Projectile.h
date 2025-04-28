#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"

class Projectile {
    public:
        Projectile(int x, int y, Enemy* target);
        void update();
        void render(SDL_Renderer* renderer);
        void hit();
        bool getHasHit() const;
        int getX() const;
        int getY() const;

    private:
        int x, y;
        Enemy* target;
        int speed = 2;
        bool hasHit = false;
};