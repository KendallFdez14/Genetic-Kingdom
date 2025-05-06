#pragma once
#include <SDL2/SDL.h> 

class Enemy {
    public:
        Enemy(int x, int y);
    
        void update();
        void render(SDL_Renderer* renderer);
        void takeDamage(int dmg);
        bool isDead() const;
        int getX() const;
        int getY() const;
    
    private:
        int x, y;
        int speed = 1;
        int health = 100; // aumentado
};