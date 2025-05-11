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
    void render(SDL_Renderer* renderer, bool isSelected = false); // Agregar parámetro para resaltar la torreta
    static int getCost(TowerType type);
    bool upgrade(int& gold); // Método para mejorar la torreta
    int getLevel() const; // Método para obtener el nivel de la torre
    bool isMouseOver(int mouseX, int mouseY) const; // Verificar si el mouse está sobre la torreta
    TowerType getType() const; // Obtener el tipo de la torre

private:
    int x, y;
    TowerType type;
    int level = 1; // Nivel inicial de la torreta

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
    void shootSpecial(Enemy& target, std::vector<std::unique_ptr<Projectile>>& projectiles);  
    void configureStats();
    bool isInRange(const Enemy& enemy);
};
