#ifndef ENEMY
#define ENEMY

#include <raylib.h>
#include <raymath.h>
#include "entity-h.hpp"
#include "tile_map-h.hpp"

class Enemy;

class EnemyState {
public:
    virtual ~EnemyState() {}
    virtual void Enter(Enemy& enemy) = 0;
    virtual void Update(Enemy& enemy, float delta_time) = 0;
    virtual void HandleCollision(Enemy& enemy, Entity* other_entity) = 0;
};

class EnemyWandering : public EnemyState {
public:
    float change_direction_cooldown;
    Vector2 move_direction;
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
    void HandleCollision(Enemy& enemy, Entity* other_entity);
};

class EnemyChasing : public EnemyState {
public:
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
    void HandleCollision(Enemy& enemy, Entity* other_entity);
};

class EnemyReady : public EnemyState {
public:
    float ready_timer;
    Vector2 aim_direction;
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
    void HandleCollision(Enemy& enemy, Entity* other_entity);
};

class EnemyAttacking : public EnemyState {
public:
    Vector2 attack_direction;
    void Enter(Enemy& enemy);
    void Update(Enemy& enemy, float delta_time);
    void HandleCollision(Enemy& enemy, Entity* other_entity);
};

class Enemy : public Entity {
public:
    TileMap* tile_map;
    float rotation;
    Color color;

    bool active;

    float detection_radius;
    float aggro_radius;
    float ready_attack_radius;

    Vector2 velocity;
    Vector2 acceleration;
    float speed;

    Entity* entity_following;

    EnemyWandering wandering;
    EnemyChasing chasing;
    EnemyReady ready;
    EnemyAttacking attacking;

    void setTileMap(TileMap* map) {
        tile_map = map;
    } 

    Enemy(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp);

    void Update(float delta_time);

    void Draw();

    void SetState(EnemyState* new_state);

    void HandleCollision(Entity* other_entity);

    EnemyState* current_state;

private:
};

#endif