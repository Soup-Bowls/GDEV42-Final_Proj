#ifndef TILEMAP 
#define TILEMAP 

#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "entity-h.hpp"
#include "scene_manager.hpp"

using namespace std;

class TileMap;

struct Tile {
    Rectangle tileLocation;
    bool hasCollision;
};

class TileMap : public Entity{
public: 
    Texture2D tileset;
    vector<Tile> tileList;
    int tilemap[100][100]; 
    int mapWidth, mapHeight;
    int TILE_COUNT;
    Vector2 playerPos, enemyPos, enemyPos2, enemyPos3;

    // Updated to use ResourceManager
    void LoadTilemapData(const char* filename, ResourceManager* resource_manager = nullptr);
    void DrawTilemap();
    bool CheckTileCollision(Entity* entity);

};

#endif