#ifndef SAVE_SYSTEM_HPP
#define SAVE_SYSTEM_HPP

#include <string>
#include <iostream>
#include <fstream>

class SaveSystem {
public:
    static SaveSystem* GetInstance() {
        static SaveSystem instance;
        return &instance;
    }

    bool SaveWave(int wave) {
        try {
            std::ofstream saveFile("savegame.txt");
            if (saveFile.is_open()) {
                saveFile << wave;
                saveFile.close();
                std::cout << "Saved wave: " << wave << std::endl;
                return true;
            } else {
                std::cerr << "Unable to open save file for writing" << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error saving wave: " << e.what() << std::endl;
            return false;
        }
    }

    int LoadWave() {
        try {
            std::ifstream saveFile("savegame.txt");
            if (saveFile.is_open()) {
                int wave = 1;  // Default to wave 1 if reading fails
                saveFile >> wave;
                saveFile.close();
                std::cout << "Loaded wave: " << wave << std::endl;
                return wave;
            } else {
                std::cout << "No save file found, starting from wave 1" << std::endl;
                return 1;  // Default to wave 1 if no save file exists
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading wave: " << e.what() << std::endl;
            return 1;  // Default to wave 1 on error
        }
    }

    bool HasSaveFile() {
        std::ifstream saveFile("savegame.txt");
        return saveFile.good();
    }

private:
    SaveSystem() {}  // Private constructor for singleton
    SaveSystem(const SaveSystem&) = delete;
    void operator=(const SaveSystem&) = delete;
};

#endif