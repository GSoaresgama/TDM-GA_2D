#pragma once

#include <iostream>
#include "math.h"
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Dense>

class Player;

#define NUMBER_OF_PLAYERS 3

//----------------PLAYER ID----------------
#define NOTHING 1
#define LIGHT_ASSAULT 11
#define ASSAULT 12
#define SNIPER 13
#define OBSTACLE 5

#define ALLY 3
#define ENEMY -3

//----------------PLAYER STATUS----------------
#define PLAYER_STATUS_INTERVAL 5
const float playerStatusInterval = PLAYER_STATUS_INTERVAL / 3.0;
const float playerStatusLA = 0;
const float playerStatusA = playerStatusInterval;
const float playerStatusS = 2 * playerStatusInterval;

//----------------PLAYERS VISION----------------
const float ANGLE_INTERVAL = 0.15;
const int NUMBER_OF_ANGLES_INTERVAL = 2 * M_PI / ANGLE_INTERVAL;
const int NUMBER_OF_ANGLES_TO_CHECK = NUMBER_OF_ANGLES_INTERVAL / 2;

//----------------PLAYERS PARAMTERS----------------
#define RADIUS 10
#define RADIUS_OFFSET 1

const int _RADIUS_TOTAL_DISTANCE = RADIUS + RADIUS_OFFSET;
const int safeDist = 2 * RADIUS + RADIUS_OFFSET;

//----------------TOUCH SENSORS----------------

const int TOUCH_SENSOR_DIST = 25;
const int NUMBER_OF_TOUCH_SENSORS = 10;
const float TOUCH_ANGLE_INTERVAL = 2.0 * M_PI / NUMBER_OF_TOUCH_SENSORS;

//----------------COLORS----------------
const cv::Scalar LIGHT_ASSAULT_COLOR = cv::Scalar(0, 255, 0);  //green
const cv::Scalar SNIPER_COLOR = cv::Scalar(0, 0, 255);         //red
const cv::Scalar ASSAULT_COLOR = cv::Scalar(255, 0, 0);        //blue

//----------------RAY COLORS----------------
const cv::Scalar LIGHT_ASSAULT_RAY = cv::Scalar(1, 255, 0);  //green ray
const cv::Scalar ASSAULT_RAY = cv::Scalar(255, 1, 0);        //blue ray
const cv::Scalar SNIPER_RAY = cv::Scalar(1, 0, 255);         //red ray

const cv::Point aux = cv::Point(-RADIUS / 2, RADIUS / 2);  //offset to print text

#include "ANN.h"
#include "Screen.h"

const int MAX_TIME_STAND = 20;

using namespace Eigen;

typedef struct enemyInfo_t {
    cv::Point posiAprox;
    int playerType;

} enemyInfo_t;

class Player {
   protected:
    cv::Point center;  //position
    cv::Point movePt;

    cv::Scalar playerColor;  //color
    cv::Scalar playerRay;    //color

    std::string playerIDStr;  //id->str

    bool alive;

    int playerType;  //type of player
    int playerID;    //number of player
    float life;      //current life
    int damage;      //player damage

    int shotInterval;
    int timeShot;

    //----------------RAY INFO----------------
    int *raysID;
    int *raysDist;

    int *touchRayID;

    float score;  //player score

    //----------------VISION INFO----------------
    int visionDist;
    float visionAngle;
    float direction;
    float speedLimit;
    float angularSpeedLimit;
    float lastAngularSpeed;

    double separationAngle;  //offset to next ray
    double angleCorrection;  //corrects rays positions

    int lastPlayerType;
    float playerStatus;  //player type changes dynamically

    Screen *screen;

    cv::Point **playersCenter;

    vectorF *input;

   public:
    cv::Point initialPos;

    int numberOfRays;

    ANN *ann;

    int ANNInputSize;
    int ANNOutputSize;

    vectorF *output;

    explicit Player();
    ~Player();

    //----------------GET & SET----------------
    bool isAlive();

    inline int getPlayerID() {
        return playerID;
    }
    inline int getPlayerType() {
        return playerType;
    }
    inline int getLife() {
        return life;
    }
    inline int getDamage() {
        return damage;
    }
    inline cv::Point getCenter() {
        return center;
    }
    inline cv::Point *getCenterPtr() {
        return &center;
    }
    inline float getScore() {
        return score;
    }

    inline void updateScore(float change) {
        score += change;
    }
    inline void setScore(float newScore) {
        score = newScore;
    }

    void setAlive(bool alive);

    //----------------INICIAL VALUES----------------
    void setPlayerValues(Screen *screen, int playerID, cv::Point **playersCenter);  //inicial values
    void setPosition();                                                                                                       //initial position
    int checkPosition();

    //----------------DRAW----------------
    void drawPlayer();                                 //draws player
    void drawVisionLines(float currentAngle, int id);  //draw vision lines

    //----------------VISION----------------
    void updateVision();  //updates vision info

    //----------------MOVEMENT----------------
    void move();  //move player
    int checkMove(cv::Point offset);

    //----------------CONFLICTS----------------
    void takeDamage(int damage);  //get shot
    enemyInfo_t killPlayer();     //shot playe

    //----------------ANN----------------
    void setComunInput();

    //----------------RESET----------------
    void reset(float life, bool resetScore);
};

#include "Assault.h"
#include "Light_Assault.h"
#include "Sniper.h"