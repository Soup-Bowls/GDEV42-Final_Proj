#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <raymath.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "scene_manager.hpp"

class TileMap;
class Player;
class Enemy;

class Level : public Scene {
public:
    Level();
    ~Level();
    
    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;
    
    void MoveCamera(Camera2D* cam, Player* player, float delta_time);
    
private:
    static const int WINDOW_WIDTH = 1280;
    static const int WINDOW_HEIGHT = 720;
    
    Rectangle camera_window;
    float cam_drift;
    float cam_zoom;
    bool zoom_in;
    
    bool game_ongoing;
    bool enemy_lose;
    
    TileMap* map;
    Player* player;
    std::vector<Enemy> enemies;
    
    Camera2D camera_view;

    bool testing_mode = false;
};

#endif 