#ifndef SETTINGS_SCENE_H
#define SETTINGS_SCENE_H

#include "scene_manager.hpp"
#include "main_menu_scene-h.hpp"
#include <raylib.h>
#include <algorithm>
#include <vector>

// Slider UI Component
class VolumeSlider : public UIComponent {
public:
    VolumeSlider(Rectangle bounds, const char* label, float initialValue);

    void Draw() override;
    bool HandleClick(Vector2 click_position) override;
    float GetValue() const { return currentValue; }
    void SetValue(float value) { 
    currentValue = std::clamp(value, 0.0f, 1.0f); 
    }

private:
    const char* label;
    float currentValue;  // 0.0 to 1.0
    Color sliderColor = GRAY;
    Color handleColor = RED;
};

class SettingsScene : public Scene {
public:
    SettingsScene();
    ~SettingsScene();

    void Begin() override;
    void End() override;
    void Update() override;
    void Draw() override;

private:
    void SaveSettings();
    void LoadSettings();

    std::vector<UIComponent*> uiElements;
    std::vector<MenuButton*> buttons;
    Texture settingsBackground;
    Music menu_theme = {0};
    bool musicLoaded = false;
    VolumeSlider* masterVolumeSlider;
    VolumeSlider* musicVolumeSlider;
    VolumeSlider* sfxVolumeSlider;

    void InitializeUI();
    void UpdateVolumes();
};


#endif