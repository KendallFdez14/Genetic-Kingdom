#include "Tower.h"
#include "Projectile.h"
#include <cmath>
#include <cstdlib> // Para rand() y srand()
#include <ctime>   // Para inicializar la semilla de números aleatorios

Tower::Tower(int x, int y, TowerType type)
    : x(x), y(y), type(type) {
    configureStats(); // Configurar atributos según el tipo de torre
    currentSpecialCooldown = specialCooldown;

    // Inicializar la semilla de números aleatorios
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void Tower::configureStats() {
    switch (type) {
        case TowerType::Archer:
            damage = 10;
            range = 150; // Rango en píxeles
            fireRate = 25; // Dispara cada 30 frames (~0.5 segundos a 60 FPS)
            cost = 10;

            // Configuración del ataque especial
            specialDamage = 20;
            specialCooldown = 300; // 5 segundos a 60 FPS
            currentSpecialCooldown = 0;
            break;
        case TowerType::Mage:
            damage = 20;
            range = 120;
            fireRate = 55; // Dispara cada 60 frames (~1 segundo a 60 FPS)
            cost = 20;

            specialDamage = 40;
            specialCooldown = 420; // 7 segundos
            currentSpecialCooldown = 0;
            break;
        case TowerType::Artillery:
            damage = 50;
            range = 200;
            fireRate = 85; // Dispara cada 90 frames (~1.5 segundos a 60 FPS)
            cost = 30;

            specialDamage = 100;
            specialCooldown = 600; // 10 segundos
            currentSpecialCooldown = 0;
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

void Tower::shootSpecial(Enemy& target, std::vector<std::unique_ptr<Projectile>>& projectiles) {
    switch (type) {
        case TowerType::Archer:
            // Disparar 3 proyectiles normales en diferentes direcciones
            projectiles.emplace_back(std::make_unique<Arrow>(x, y, &target, specialDamage));
            projectiles.emplace_back(std::make_unique<Arrow>(x + 10, y, &target, specialDamage));
            projectiles.emplace_back(std::make_unique<Arrow>(x - 10, y, &target, specialDamage));
            break;
        case TowerType::Mage:
            // Disparar un proyectil especial más grande
            projectiles.emplace_back(std::make_unique<SpecialProjectile>(x, y, &target, specialDamage));
            break;
        case TowerType::Artillery:
            // Aumentar la velocidad de ataque por 2 segundos
            fireRate /= 8; // Duplicar la velocidad de ataque
            specialEffectDuration = 120; // Duración del efecto especial (2 segundos a 60 FPS)
            break;
    }
}

void Tower::update(std::vector<std::unique_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles) {
    // Reducir cooldowns
    if (fireCooldown > 0) fireCooldown--;
    if (currentSpecialCooldown > 0) currentSpecialCooldown--;

    // Reducir la duración del efecto especial del artillero
    if (specialEffectDuration > 0) {
        specialEffectDuration--;
        if (specialEffectDuration == 0) {
            fireRate *= 2; // Restaurar la velocidad de ataque original
        }
    }

    // Si el ataque especial está listo, usarlo con una probabilidad del 50%
    if (currentSpecialCooldown <= 0) {
        if (std::rand() % 100 < 50) { // Generar un número entre 0 y 99, 50% de probabilidad
            for (auto& enemy : enemies) {
                if (isInRange(*enemy)) {
                    shootSpecial(*enemy, projectiles);
                    currentSpecialCooldown = specialCooldown; // Reiniciar el cooldown del ataque especial
                    return;
                }
            }
        }
    }

    // Si el ataque normal está listo, usarlo
    if (fireCooldown <= 0) {
        for (auto& enemy : enemies) {
            if (isInRange(*enemy)) {
                shoot(*enemy, projectiles);
                fireCooldown = fireRate; // Reiniciar el cooldown del ataque normal
                break;
            }
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