#pragma once
#include <cmath>

//----------------ASSAULT VALUES----------------
#define ASSAULT_HEALTH 12
#define ASSAULT_DAMAGE 4
#define ASSAULT_NUMBER_OF_RAYS 12
#define ASSAULT_SPEED_LIMIT 7.0
#define ASSAULT_VISION_DIST 180
#define ASSAULT_SHOT_INTERVAL 6

const float ASSAULT_ANGULAR_SPEED_LIMIT = M_PI / 45;
const float ASSAULT_VISION_ANGLE = M_PI / 2.5;

class Assault;

#include "Player.h"

class Assault : public Player {
   private:
   public:
    Assault();
    ~Assault();
};
