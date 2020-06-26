#pragma once

#include <iostream>
#include "math.h"
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Dense>

class Player;

#define NUMBER_OF_PLAYERS 3

//----------------PLAYER ID----------------
#define NOTHING 10
#define LIGHT_ASSAULT 1
#define ASSAULT 2
#define SNIPER 3
#define OBSTACLE 50

#define ALLY 30
#define ENEMY -30

//----------------PLAYER STATUS----------------
#define PLAYER_STATUS_INTERVAL 5
const float playerStatusInterval = PLAYER_STATUS_INTERVAL/3.0;
const float playerStatusLA = 0;
const float playerStatusA =  playerStatusInterval;
const float playerStatusS = 2*playerStatusInterval;

//----------------VISION AND PLAYERS PARAMTERS----------------
#define RADIUS 10
#define RADIUS_OFFSET 1

const int _RADIUS_TOTAL_DISTANCE = RADIUS + RADIUS_OFFSET;
const int safeDist = 2 * RADIUS + RADIUS_OFFSET;

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

    bool alive;  //player status

    int playerType;  //type of player
    int playerID;    //number of player
    int life;        //current life
    int damage;  //player damage

    int shotInterval;
    int timeShot;

    //----------------RAY INFO----------------
    int *raysID;
    int *raysDist;

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

    float playerStatus; //player type changes dynamically

    Screen *screen;

    cv::Point **playersCenter;

    // VectorXf *input;
    vectorF *input;

   public:
    cv::Point initialPos;

    int numberOfRays;

    ANN *ann;

    int ANNInputSize;
    int ANNOutputSize;

    // VectorXf *output;
    vectorF *output;

    explicit Player();
    ~Player();

    //gets:
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

    //initial values
    void setPlayerValues(Screen *screen, int playerID, cv::Point **playersCenter);  //inicial values
    void setPosition();                                                                       //initial position
    int checkPosition();

    //draw
    void drawPlayer();                                  //draws player
    void drawVisionLines(double currentAngle, int id);  //draw vision lines

    //vision
    void updateVision();  //updates vision info

    //movement
    void move();  //move player
    int checkMove(cv::Point offset);

    //conflicts
    void takeDamage(int damage);            //get shot
    enemyInfo_t killPlayer();  //shot playe

    //ANN:
    void setComunInput();

    //reset
    void reset(int life, bool resetScore);
};

#include "Light_Assault.h"
#include "Sniper.h"
#include "Assault.h"