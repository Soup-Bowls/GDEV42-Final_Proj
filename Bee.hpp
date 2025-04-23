#ifndef BEE
#define BEE

#include <raylib.h>
#include <raymath.h>
#include "Entity.hpp"
#include "EnemyBase.hpp"
#include "TileMap.hpp"

#define GAME_SCENE_SPRITE_BEE "Assets/Sprites/bee.png"

class Bee;

class BeeState {
public:
    virtual ~BeeState() {}
    virtual void Enter(Bee& bee) = 0;
    virtual void Update(Bee& bee, float delta_time) = 0;
    virtual void HandleCollision(Bee& bee, Entity* other_entity) = 0;
};

class BeeWandering : public BeeState {
public:
    float change_direction_cooldown;
    Vector2 move_direction;
    void Enter(Bee& bee);
    void Update(Bee& bee, float delta_time);
    void HandleCollision(Bee& bee, Entity* other_entity);
};

class BeeChasing : public BeeState {
public:
    void Enter(Bee& bee);
    void Update(Bee& bee, float delta_time);
    void HandleCollision(Bee& bee, Entity* other_entity);
};

class BeeReady : public BeeState {
public:
    float ready_timer;
    Vector2 aim_direction;
    void Enter(Bee& bee);
    void Update(Bee& bee, float delta_time);
    void HandleCollision(Bee& bee, Entity* other_entity);
};

class BeeAttacking : public BeeState {
public:
    Vector2 attack_direction;
    void Enter(Bee& bee);
    void Update(Bee& bee, float delta_time);
    void HandleCollision(Bee& bee, Entity* other_entity);
};

class Bee : public BaseEnemy {
public:

    enum class Animation_type {
        WANDERING, 
        CHARGING, 
        ATTACKING
    };

    Animation_type animation_state;

    Rectangle beeFrameRect;
    Rectangle beeDR;


    BeeWandering wandering;
    BeeChasing chasing;
    BeeReady ready;
    BeeAttacking attacking;

    void setTileMap(TileMap* map) {
        tile_map = map;
    } 

    Bee(Vector2 pos, float spd, float rad, float d_radius, float a_radius, float r_radius, int hp);

    void Update(float delta_time) override;

    void Draw() override;

    void SetState(BeeState* new_state);

    void HandleCollision(Entity* other_entity) override;

private:
    BeeState* current_state;
    bool flash_visible;
    float flash_timer;
    float flash_interval;

};

#endif