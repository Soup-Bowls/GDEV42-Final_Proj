#ifndef LEVEL_H
#define LEVEL_H

#include <raylib.h>
#include <raymath.h>
#include <cstdlib>
#include <vector>
#include <string>
#include "scene_manager.hpp"

// Forward declarations
class TileMap;
class Player;
class Enemy;

class Level : public Scene {
public:
    Level();
    ~Level();
    
    // Scene interface methods
    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;
    
    // Level-specific methods
    void MoveCamera(Camera2D* cam, Player* player, float delta_time);
    
private:
    // Window dimensions - now matches your main.cpp
    static const int WINDOW_WIDTH = 1280;
    static const int WINDOW_HEIGHT = 720;
    
    // Camera window for player movement boundaries
    Rectangle camera_window;
    float cam_drift;
    float cam_zoom;
    bool zoom_in;
    
    // Game state
    bool game_ongoing;
    bool enemy_lose;
    
    // Game objects
    TileMap* map;
    Player* player;
    std::vector<Enemy> enemies;
    
    // Camera
    Camera2D camera_view;

    bool testing_mode = false;
};

#endif 