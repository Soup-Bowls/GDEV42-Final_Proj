#ifndef HIGH_SCORE_HPP
#define HIGH_SCORE_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>

#define MAX_HIGH_SCORES 10

struct HighScoreEntry {
    std::string playerName;
    int score;
    
    HighScoreEntry(const std::string& name = "???", int scoreValue = 0) 
        : playerName(name), score(scoreValue) {}
        
    bool operator<(const HighScoreEntry& other) const {
        return score > other.score; // Sort in descending order
    }
};

class HighScoreManager {
private:
    std::vector<HighScoreEntry> highScores;
    std::string fileName;
    
public:
    HighScoreManager(const std::string& file = "highscores.dat") 
        : fileName(file) {
        LoadHighScores();
    }
    
    void LoadHighScores() {
        highScores.clear();
        std::ifstream file(fileName);
        if (file.is_open()) {
            std::string name;
            int score;
            while (file >> name >> score) {
                highScores.push_back(HighScoreEntry(name, score));
            }
            file.close();
            std::sort(highScores.begin(), highScores.end()); // Sort by score (descending)
            std::cout << "Loaded " << highScores.size() << " high scores" << std::endl;
        } else {
            std::cout << "No high score file found. Starting with empty list." << std::endl;
        }
    }
    
    void SaveHighScores() {
        std::ofstream file(fileName);
        if (file.is_open()) {
            for (const auto& entry : highScores) {
                file << entry.playerName << " " << entry.score << std::endl;
            }
            file.close();
            std::cout << "Saved " << highScores.size() << " high scores" << std::endl;
        } else {
            std::cerr << "Error: Could not save high scores to file." << std::endl;
        }
    }
    
    bool IsHighScore(int score) const {
        if (highScores.size() < MAX_HIGH_SCORES) {
            return true; // Always a high score if we don't have MAX_HIGH_SCORES yet
        }
        
        return score > highScores.back().score;
    }
    
    void AddHighScore(const std::string& name, int score) {
        highScores.push_back(HighScoreEntry(name, score));
        std::sort(highScores.begin(), highScores.end()); // Sort by score (descending)
        
        // Keep only the top MAX_HIGH_SCORES
        if (highScores.size() > MAX_HIGH_SCORES) {
            highScores.resize(MAX_HIGH_SCORES);
        }
        
        SaveHighScores();
    }
    
    const std::vector<HighScoreEntry>& GetHighScores() const {
        return highScores;
    }
};

#endif // HIGH_SCORE_HPP
