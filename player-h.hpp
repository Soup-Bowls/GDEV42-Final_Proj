#ifndef PLAYER
#define PLAYER

#include <raylib.h>
#include <raymath.h>
#include "entity-h.hpp"
#include "tile_map-h.hpp"

class Player;

// Base Player State
// All Player States inherit this interface
class PlayerState {
public:
    virtual ~PlayerState() {}
    virtual void Enter(Player& player) = 0;
    virtual void Update(Player& player, float delta_time) = 0;
    virtual void HandleCollision(Player& player, Entity* other_entity) = 0;
};

class PlayerIdle : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerMoving : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerBlocking : public PlayerState {
public:
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerAttacking : public PlayerState {
public:
    float active_time;
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};

class PlayerDodging : public PlayerState {
public:
    Vector2 dodge_direction;
    void Enter(Player& player);
    void Update(Player& player, float delta_time);
    void HandleCollision(Player& player, Entity* other_entity);
};


// Base Player Class
class Player : public Entity {
public:
    TileMap*  tile_map;
    Vector2 velocity;
    Vector2 acceleration;
    float speed;
    
    bool in_attacking;
    float attack_radius;

    void setTileMap(TileMap* map) {
        tile_map = map;
    }
    Player(Vector2 pos, float rad, float spd, int hp);

    void Update(float delta_time);

    void Draw();

    void SetState(PlayerState* new_state);

    void HandleCollision(Entity* other_entity);

    PlayerIdle idle;
    PlayerMoving moving;
    PlayerBlocking blocking;
    PlayerAttacking attacking;
    PlayerDodging dodging;

private:
    PlayerState* current_state;
};


#endif