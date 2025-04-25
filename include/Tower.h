#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"
#include <vector>

class Tower {
public:
    Tower(int x, int y);
    void update(std::vector<Enemy>& enemies); 
    void render(SDL_Renderer* renderer);

private:
    int x, y;
    int range = 300;
    int fireRate = 20; // ticks entre disparos (~1 seg)
    int fireCooldown = 0;

    void shoot(Enemy& target);
    bool isInRange(const Enemy& enemy);
};
