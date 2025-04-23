#ifndef SCENE_MANAGER
#define SCENE_MANAGER

#include <raylib.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

class SceneManager;

// Base class that all scenes inherit
class Scene {
    // Reference to the scene manager.
    // In practice, you would want to make this private (or protected)
    // and set this via the constructor.
protected:
    SceneManager* scene_manager = nullptr;

public:
    // Begins the scene. This is where you can load resources
    virtual void Begin() = 0;

    // Ends the scene. This is where you can unload resources
    virtual void End() = 0;

    // Updates scene's state (physics, input, etc. can be added here)
    virtual void Update() = 0;

    // Draws the scene's current state
    virtual void Draw() = 0;

    void SetSceneManager(SceneManager* scene_manager) {
        this->scene_manager = scene_manager;
    }

    SceneManager* GetSceneManager() {
        return scene_manager;
    }
};

class SceneManager {
    // Mapping between a scene ID and a reference to the scene
    std::unordered_map<int, Scene*> scenes;

     // Current active scene
    Scene* active_scene = nullptr;

public:
    // Adds the specified scene to the scene manager, and assigns it
    // to the specified scene ID
    void RegisterScene(Scene* scene, int scene_id) {
        scenes[scene_id] = scene;
    }

    // Removes the scene identified by the specified scene ID
    // from the scene manager
    void UnregisterScene(int scene_id) {
        scenes.erase(scene_id);
    }

    // Switches to the scene identified by the specified scene ID.
    void SwitchScene(int scene_id) {
    // Extensive logging and error checking
    std::cout << "Attempting to switch to scene: " << scene_id << std::endl;

    // If the scene ID does not exist in our records,
    // don't do anything (or you can print an error message).
    if (scenes.find(scene_id) == scenes.end()) {
        std::cerr << "ERROR: Scene ID " << scene_id << " not found" << std::endl;
        return;
    }

    try {
        // If there is already an active scene, end it first
        if (active_scene != nullptr) {
            std::cout << "Ending current active scene" << std::endl;
            active_scene->End();
        }

        // Retrieve the new scene
        Scene* new_scene = scenes[scene_id];
        if (new_scene == nullptr) {
            std::cerr << "ERROR: Retrieved scene is null for ID " << scene_id << std::endl;
            return;
        }

        std::cout << "Setting new active scene" << std::endl;
        active_scene = new_scene;

        std::cout << "Beginning new scene" << std::endl;
        active_scene->Begin();

        std::cout << "Successfully switched to scene " << scene_id << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR during scene switch: " << e.what() << std::endl;
        // Potentially set a default scene or handle the error
        active_scene = nullptr;
    }
    catch (...) {
        std::cerr << "UNKNOWN ERROR during scene switch" << std::endl;
        active_scene = nullptr;
    }
}

    // Gets the active scene
    Scene* GetActiveScene() {
        return active_scene;
    }

    // close window

    //exit flag double check
    void CancelExit() {
        shouldExit = false;
    }

    void RequestGameExit() {
        shouldExit = true;
    }

    bool ShouldExit() const {
        return shouldExit;
    }

    bool shouldExit = false;
};

//--------------------------------------

struct TextureData {
    Texture texture;
    int width;
    int height;
};

class ResourceManager {
std::unordered_map<std::string, TextureData> textures; // Store TextureData

    ResourceManager() {}
private:

    std::unordered_map<std::string, int> textureReferences;

public:
    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

    static ResourceManager* GetInstance() {
        static ResourceManager instance;
        return &instance;
    }

    TextureData GetTexture(const std::string& path) {
        try {
            // Check if the texture is already loaded
            if (textures.find(path) != textures.end()) {
                std::cout << "Resource Already Loaded: " << path << std::endl;
                textureReferences[path]++;
                return textures[path]; 
            }
    
            std::cout << "Attempting to load texture: " << path << std::endl;
            
            // Check if file exists first
            if (!FileExists(path.c_str())) {
                std::cerr << "ERROR: Texture file does not exist: " << path << std::endl;
                TextureData emptyData = {0};
                return emptyData;
            }
    
            // Load the texture
            Texture texture = LoadTexture(path.c_str());
    
            // Error handling
            if (texture.id == 0) {
                std::cerr << "Failed to load texture " << path << std::endl;
                TextureData emptyData = {0};
                return emptyData;
            }
    
            std::cout << "Loaded " << path << " from Disk" << std::endl;
            TextureData textureData;
            textureData.texture = texture;
            textureData.width = texture.width;
            textureData.height = texture.height;
            
            textures[path] = textureData;
            textureReferences[path] = 1;
            return textureData;
        }
        catch (const std::exception& e) {
            std::cerr << "Exception in GetTexture: " << e.what() << std::endl;
            TextureData emptyData = {0};
            return emptyData;
        }
        catch (...) {
            std::cerr << "Unknown exception in GetTexture" << std::endl;
            TextureData emptyData = {0};
            return emptyData;
        }
    }

    void UnloadAllTextures() {
        for (auto& it : textures) {
            UnloadTexture(it.second.texture);
        }
        textures.clear();
    }

    void UnloadTextures(const std::string& path) {

        if (textureReferences.find(path) != textureReferences.end()) {
            textureReferences[path]--;
            
            if (textureReferences[path] <= 0) {
                UnloadTexture(textures[path].texture);
                textures.erase(path);
                textureReferences.erase(path);
            }
        }
    }

};

//-------------------------

class AudioManager {
public:
    static AudioManager* GetInstance() {
        static AudioManager instance;
        return &instance;
    }

    void SetMasterVolume(float volume) {
        // Ensure volume is between 0.0 and 1.0
        master_volume = std::clamp(volume, 0.0f, 1.0f);
        
        // Apply volume to all music and sound effects
        SetMusicVolume(current_music, master_volume);
        SetSoundVolume(all_sounds, master_volume);
    }

    float GetMasterVolume() const {
        return master_volume;
    }

    void SetCurrentSound(Sound sound) {
        all_sounds = sound;
        SetSoundVolume(all_sounds, master_volume);
    }

    void SetCurrentMusic(Music music) {
        current_music = music;
        SetMusicVolume(current_music, master_volume);
    }

private:
    AudioManager() : master_volume(0.5f) {}  // Default 50% volume
    
    float master_volume;
    Music current_music;
    Sound all_sounds;
};

#endif