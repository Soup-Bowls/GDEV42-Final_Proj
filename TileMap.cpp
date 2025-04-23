#include <raylib.h>
#include <raymath.h>

#include "TileMap.hpp"

void TileMap::LoadTilemapData(const char* filename) {
    ifstream file(filename);

    string tilesetFile;
    file >> tilesetFile;
    tileset = LoadTexture(tilesetFile.c_str());

    file >> TILE_COUNT;
    tileList.resize(TILE_COUNT);

    // cout << "Tile count: " << TILE_COUNT << endl;

    // Read tile positions
    for (int i = 0; i < TILE_COUNT; i++) {
        file >> tileList[i].tileLocation.x >> tileList[i].tileLocation.y
             >> tileList[i].tileLocation.width >> tileList[i].tileLocation.height
             >> tileList[i].hasCollision;
    }

    file >> mapWidth >> mapHeight;
    cout << "Map size: " << mapWidth << "x" << mapHeight << endl;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            file >> tilemap[y][x];
        }
        cout << endl;
    }

    file >> playerPos.x >> playerPos.y;
    cout << "Player position: " << playerPos.x << " " << playerPos.y << endl;

    file >> enemyPos.x >> enemyPos.y;
    cout << "Enemy position: " << enemyPos.x << " " << enemyPos.y << endl;
    file >> enemyPos2.x >> enemyPos2.y;
    cout << "Enemy position2: " << enemyPos2.x << " " << enemyPos2.y << endl;
    file >> enemyPos3.x >> enemyPos3.y;
    cout << "Enemy position3: " << enemyPos3.x << " " << enemyPos3.y << endl;

    file.close();
}

void TileMap::DrawTilemap() {
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int tileIndex = tilemap[y][x];
            if (tileIndex < 0 || tileIndex >= TILE_COUNT) continue;

            Rectangle src = tileList[tileIndex].tileLocation;
            Rectangle dest = { x * 16.0f, y * 16.0f, 16, 16 };

            DrawTexturePro(tileset, src, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
    }
}

bool TileMap::CheckTileCollision(Entity* entity){
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int tileIndex = tilemap[y][x];

            if (tileIndex < 0 || tileIndex >= TILE_COUNT) continue;

            if (tileList[tileIndex].hasCollision) {
                Rectangle tileRect = { x * 16.0f, y * 16.0f, 16, 16 };

                if (CheckCollisionCircleRec(entity->position, entity->radius, tileRect)) {
                    // cout << "wall collision" << endl;
                    return true; 
                }
            }
        }
    }
    return false;
}