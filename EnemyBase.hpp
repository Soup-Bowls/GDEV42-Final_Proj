#ifndef BASE_ENEMY_HPP
#define BASE_ENEMY_HPP

#include "Entity.hpp"
#include "TileMap.hpp"

class BaseEnemy : public Entity {
public:
    int enemyID;    

    Vector2 velocity;
    Vector2 acceleration;
    float speed = 0.0f;
    bool active = true;
    Color color;
    int direction = 0;
    float animationTimer = 0.0f;
    float frameSpeed = 0.2f;
    int currentFrame = 0;
    int maxFrames = 1;
    Texture2D sprite;
    float frameWidth;
    float frameHeight;

    float detection_radius;
    float aggro_radius;
    float ready_attack_radius;

    TileMap* tile_map;
    Entity* entity_following;

    void setTileMap(TileMap* map) {
        tile_map = map;
    }

    virtual void Update(float delta_time) = 0;
    virtual void Draw() = 0;
    virtual void HandleCollision(Entity* other_entity) = 0;

    virtual ~BaseEnemy() {}
};

#endif
