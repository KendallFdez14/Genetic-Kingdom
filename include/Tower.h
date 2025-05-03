#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"
#include "Projectile.h"
#include <vector>

enum class TowerType {
    Archer,
    Mage,
    Artillery
};

class Tower {
public:
    Tower(int x, int y, TowerType type);
    void update(std::vector<Enemy>& enemies, std::vector<Projectile>& projectiles);
    void render(SDL_Renderer* renderer);

private:
    int x, y;
    int range = 150;
    int fireRate = 60;
    int fireCooldown = 0;
    TowerType type;

    void shoot(Enemy& target, std::vector<Projectile>& projectiles);
    bool isInRange(const Enemy& enemy);
};
