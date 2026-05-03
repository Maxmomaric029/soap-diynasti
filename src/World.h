#ifndef WORLD_H
#define WORLD_H

#include <string>

class WorldClock {
public:
    int hours, minutes;
    float secondCounter;
    WorldClock();
    void Update(float dt);
    std::string GetTime();
};

#endif
