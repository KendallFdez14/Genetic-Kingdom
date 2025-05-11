#pragma once
#include <SDL2/SDL.h>
#include "Enemy.h"
#include "Projectile.h"
#include <vector>
#include <memory>

enum class TowerType {
    Archer,
    Mage,
    Artillery
};

class Tower {
public:
    Tower(int x, int y, TowerType type);
    void update(std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles);
    void render(SDL_Renderer* renderer);
    static int getCost(TowerType type);

private:
    int x, y;
    TowerType type;

    int damage;
    int range;
    int fireRate; // Frames entre disparos normales
    int fireCooldown = 0;

    // Atributos del ataque especial
    int specialDamage;
    int specialCooldown;        // Tiempo total de recarga
    int currentSpecialCooldown; // Contador para la recarga
    int specialEffectDuration = 0; // Duración del efecto especial (en frames)

    int cost;

    void shoot(Enemy& target, std::vector<std::unique_ptr<Projectile>>& projectiles);
    void shootSpecial(Enemy& target, std::vector<std::unique_ptr<Projectile>>& projectiles); // Nuevo método
    bool isInRange(const Enemy& enemy);
    void configureStats(); // Configurar atributos según el tipo
};
