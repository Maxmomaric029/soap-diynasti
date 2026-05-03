#include "World.h"
#include <iomanip>
#include <sstream>

WorldClock::WorldClock() : hours(8), minutes(0), secondCounter(0.0f) {}

void WorldClock::Update(float dt) {
    secondCounter += dt;
    if (secondCounter >= 1.0f) {
        secondCounter -= 1.0f;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours++;
            if (hours >= 24) hours = 0;
        }
    }
}

std::string WorldClock::GetTime() {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << minutes;
    return ss.str();
}
