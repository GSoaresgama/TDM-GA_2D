#pragma once

#include <cmath>

#define NUMBER_OF_ASSAULTS 4
#define ASSAULT_HEALTH 50
#define ASSAULT_DAMAGE 25
#define ASSAULT_NUMBER_OF_RAYS 12
#define ASSAULT_SPEED_LIMIT 2.4
#define ASSAULT_VISION_DIST 120

const float ASSAULT_ANGULAR_SPEED_LIMIT = M_PI / 45;
const float ASSAULT_VISION_ANGLE = M_PI / 2.5;

class Assault;

#include "Moderator.h"

class Assault : public Player
{
private:
    /* data */
public:
    Assault();
    ~Assault();
};
