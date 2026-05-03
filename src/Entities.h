#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include <string>

struct Customer {
    Vector2 pos;
    float patience;
    bool isWaiting;
    bool satisfied;
    int type; // 0: Poor, 1: Merchant, 2: Knight
    float speed;
    int currentFrame;
    float frameTimer;
    Rectangle spriteRect; // Source rectangle for the sprite sheet
};

struct Competitor {
    Vector2 pos;
    float influenceRadius;
    std::string guild;
};

struct Crier {
    Vector2 pos;
    float radius;
    std::string message;
};

#endif
