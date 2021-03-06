#pragma once
#include <cmath>

//----------------SNIPER VALUES----------------
#define SNIPER_HEALTH 9
#define SNIPER_DAMAGE 6
#define SNIPER_NUMBER_OF_RAYS 12
#define SNIPER_SPEED_LIMIT 5.0
#define SNIPER_VISION_DIST 250
#define SNIPER_SHOT_INTERVAL 10

const float SNIPER_ANGULAR_SPEED_LIMIT = M_PI / 40;
const float SNIPER_VISION_ANGLE = M_PI / 4.5;

class Sniper;

#include "Player.h"

class Sniper : public Player {
   private:
   public:
    Sniper();
    ~Sniper();
};
