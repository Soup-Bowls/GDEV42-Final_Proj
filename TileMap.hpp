#ifndef TILEMAP 
#define TILEMAP 

#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "Entity.hpp"

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

    void LoadTilemapData(const char* filename);
    void DrawTilemap();
    bool CheckTileCollision(Entity* entity);

};

#endif 