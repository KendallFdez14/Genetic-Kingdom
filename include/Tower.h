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
    TowerType type;

    int damage;
    int range;
    int fireRate; // cuántos frames entre disparos
    int fireCooldown = 0;

    int specialCooldown;        // total
    int currentSpecialCooldown; // en cuenta regresiva

    void shoot(Enemy& target, std::vector<Projectile>& projectiles);
    bool isInRange(const Enemy& enemy);
    void configureStats(); // según el tipo
};
