#include <raylib.h>
#include <raymath.h>

#include "tile_map-h.hpp"

void TileMap::LoadTilemapData(const char* filename, ResourceManager* resource_manager) {
    std::cout << "TileMap::LoadTilemapData - Opening file: " << filename << std::endl;
    ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    string tilesetFile;
    file >> tilesetFile;
    std::cout << "Tileset file from TileInfo.txt: " << tilesetFile << std::endl;
    
    // Use ResourceManager if provided, otherwise use direct Raylib loading
    if (resource_manager) {
        std::cout << "Using ResourceManager to load tileset" << std::endl;
        TextureData textureData = resource_manager->GetTexture(tilesetFile);
        tileset = textureData.texture;
    } else {
        std::cout << "Loading tileset directly with Raylib" << std::endl;
        tileset = LoadTexture(tilesetFile.c_str());
    }
    
    if (tileset.id == 0) {
        std::cerr << "Error: Failed to load tileset texture: " << tilesetFile << std::endl;
        file.close();
        return;
    }
    
    std::cout << "Tileset loaded successfully" << std::endl;

    file >> TILE_COUNT;
    tileList.resize(TILE_COUNT);

    std::cout << "Tile count: " << TILE_COUNT << std::endl;

    // Read tile positions
    for (int i = 0; i < TILE_COUNT; i++) {
        file >> tileList[i].tileLocation.x >> tileList[i].tileLocation.y
             >> tileList[i].tileLocation.width >> tileList[i].tileLocation.height
             >> tileList[i].hasCollision;
    }

    file >> mapWidth >> mapHeight;
    std::cout << "Map size: " << mapWidth << "x" << mapHeight << std::endl;
    
    // Input validation for map dimensions
    if (mapWidth > 100 || mapHeight > 100) {
        std::cerr << "Error: Map dimensions exceed maximum size of 100x100" << std::endl;
        mapWidth = std::min(mapWidth, 100);
        mapHeight = std::min(mapHeight, 100);
    }

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            file >> tilemap[y][x];
        }
    }

    file >> playerPos.x >> playerPos.y;
    std::cout << "Player position: " << playerPos.x << " " << playerPos.y << std::endl;

    file >> enemyPos.x >> enemyPos.y;
    std::cout << "Enemy position: " << enemyPos.x << " " << enemyPos.y << std::endl;
    file >> enemyPos2.x >> enemyPos2.y;
    std::cout << "Enemy position2: " << enemyPos2.x << " " << enemyPos2.y << std::endl;
    file >> enemyPos3.x >> enemyPos3.y;
    std::cout << "Enemy position3: " << enemyPos3.x << " " << enemyPos3.y << std::endl;

    file.close();
    std::cout << "TileMap data loaded successfully!" << std::endl;
}

void TileMap::DrawTilemap() {
    // Check if tileset was loaded
    if (tileset.id == 0) {
        std::cerr << "Error: Tileset texture not loaded" << std::endl;
        return;
    }
    
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
    if (entity == nullptr) return false;
    
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