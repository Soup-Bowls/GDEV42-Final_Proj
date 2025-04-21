#ifndef ENTITY
#define ENTITY

#include <raylib.h>
#include <raymath.h>

class Entity {
public:
    int health;
    Vector2 position;
    float radius;
    Color color;
    float invulnerable_timer;
};

#endif