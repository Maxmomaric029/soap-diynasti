#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "World.h"
#include "Entities.h"
#include <vector>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

struct Recipe {
    std::string name;
    int cost;
    int price;
    float time;
};

struct GameState {
    int gold = 1000;
    float reputation = 50.0f;
    float sellingProgress = 0.0f;
    int currentTargetIndex = -1;
    bool inCrafting = false;
    float spawnTimer = 0.0f;
    int selectedRecipe = 0;
    
    int inventory[2] = {5, 0};
    float craftingProgress = 0.0f;
    bool isCrafting = false;

    int goldEarnedToday = 0;
    int soapsSoldToday = 0;
    bool dayEnded = false;
    
    bool lateCompetitorSpawned = false; // Fix: Bug spawning
};

class Game {
public:
    Game();
    ~Game();
    void Run();

private:
    void Update();
    void Draw();
    void HandleInput();
    void ResetDay();
    void DrawHUD();

    WorldClock clock;
    GameState state;
    Camera2D camera;
    
    // Assets
    Texture2D roguesTex;
    Texture2D tilesTex;
    Texture2D itemTex;

    Vector2 playerPos;
    int playerFrame = 0;
    float playerFrameTimer = 0.0f;

    std::vector<Customer> customers;
    std::vector<Competitor> competitors;
    std::vector<Crier> criers;
    std::vector<Recipe> recipes;
};

#endif
