#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"

class Projectile {
    public:
        Projectile(int x, int y, Enemy* target, int damage);
        void update();
        void render(SDL_Renderer* renderer);
        bool hasHit() const;
    
    private:
        float x, y;
        float speed = 4.0f;
        int damage;
        Enemy* target;
        bool hit = false;
    };
    