#pragma once
#include <SDL2/SDL.h>

class Enemy {
    public:
        Enemy(int x, int y);
        void update();
        void render(SDL_Renderer* renderer);
    
        bool isDead() const;
        int getX() const;
        int getY() const;
        void takeDamage(int dmg); // nuevo
    
    private:
        int x, y;
        int speed = 1;
        int health = 5; // vida total
    };
