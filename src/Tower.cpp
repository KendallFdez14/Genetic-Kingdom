#include "Tower.h"
#include "Projectile.h"
#include <cmath>

Tower::Tower(int x, int y, TowerType type)
    : x(x), y(y), type(type) {
    configureStats(); // Configurar atributos según el tipo de torre
    currentSpecialCooldown = specialCooldown;
}

void Tower::configureStats() {
    switch (type) {
        case TowerType::Archer:
            damage = 10;
            range = 150; // Rango en píxeles
            fireRate = 25; // Dispara cada 30 frames (~0.5 segundos a 60 FPS)
            cost = 10;
            break;
        case TowerType::Mage:
            damage = 20;
            range = 120;
            fireRate = 55; // Dispara cada 60 frames (~1 segundo a 60 FPS)
            cost = 20;
            break;
        case TowerType::Artillery:
            damage = 50;
            range = 200;
            fireRate = 85; // Dispara cada 90 frames (~1.5 segundos a 60 FPS)
            cost = 30;
            break;
    }
}

bool Tower::isInRange(const Enemy& enemy) {
    float dx = enemy.getX() - x;
    float dy = enemy.getY() - y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance <= range;
}

void Tower::shoot(Enemy& target, std::vector<std::unique_ptr<Projectile>>& projectiles) {
    switch (type) {
        case TowerType::Archer:
            projectiles.emplace_back(std::make_unique<Arrow>(x, y, &target, damage));
            break;
        case TowerType::Mage:
            projectiles.emplace_back(std::make_unique<Magic>(x, y, &target, damage));
            break;
        case TowerType::Artillery:
            projectiles.emplace_back(std::make_unique<Bullet>(x, y, &target, damage));
            break;
    }
}

void Tower::update(std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles) {
    // Reducir el cooldown del disparo
    if (fireCooldown > 0) {
        fireCooldown--;
    }

    // Si el cooldown no ha terminado, no puede disparar
    if (fireCooldown > 0) {
        return;
    }

    // Buscar un enemigo en rango
    for (auto& enemy : enemies) {
        if (isInRange(*enemy)) { // Desreferenciar el puntero único
            shoot(*enemy, projectiles); // Pasar el enemigo como referencia
            fireCooldown = fireRate; // Reiniciar el cooldown
            break; // Solo dispara a un enemigo por actualización
        }
    }
}

void Tower::render(SDL_Renderer* renderer) {
    switch (type) {
        case TowerType::Archer: SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break;
        case TowerType::Mage: SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); break;
        case TowerType::Artillery: SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255); break;
    }
    SDL_Rect rect = { x - 10, y - 10, 20, 20 };
    SDL_RenderFillRect(renderer, &rect);
}

int Tower::getCost(TowerType type) {
    switch (type) {
        case TowerType::Archer: return 10;
        case TowerType::Mage: return 20;
        case TowerType::Artillery: return 30;
    }
}