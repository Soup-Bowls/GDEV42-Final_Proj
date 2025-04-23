#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <raylib.h>
#include <raymath.h>

class Projectile {
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
    bool active;

    Texture2D projectileSprite;

    Projectile(Vector2 pos, Vector2 vel, float r, Texture2D orb, Color col);

    void Update(float dt);
    void Draw();
};

#endif
