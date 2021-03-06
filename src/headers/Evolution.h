#pragma once
#include <eigen3/Eigen/Dense>

#include "Moderator.h"

using namespace Eigen;

//----------------TEAMS INICIAL POSITIONS----------------
const cv::Point initialPos[] = {cv::Point(LENGTH - 300, HEIGHT - 250), cv::Point(LENGTH - 400, 150), cv::Point(0, 150)};

//----------------EVOLUTION VALUES----------------
#define POP_SIZE 15
const int TOURNAMENT_K = 2;
typedef struct scoreData_t {
    //----------------BEST SCORES----------------
    float BLAS;
    float BSS;
    float BAS;
    //----------------MEDIUN SOCRES----------------
    float MLAS;
    float MSS;
    float MAS;
} scoreData_t;

class Evolution {
   private:
    int TOTAL_NUMBER_OF_PLAYERS;

    //----------------TRAINING MODERATORS----------------
    Moderator *lightAssaultTraining;
    Moderator *snipersTraining;
    Moderator *assaultsTraining;

    //----------------TRAINING PLAYERS----------------
    Player **allLightAssaults;
    Player **allSnipers;
    Player **allAssaults;

    //----------------TRAINING CHILDS----------------
    ANN *lightAssaultChilds;
    ANN *snipersChilds;
    ANN *assaultsChilds;

   public:
    //----------------TRAINING TEAM SCORES----------------
    float bestLightAssaultTeamScore;
    float bestSniperTeamScore;
    float bestAssaultTeamScore;

    Moderator *bestTeams;

    //----------------BEST TEAMS PLAYERS----------------
    Moderator *bestLightAssaults;
    Moderator *bestSnipers;
    Moderator *bestAssaults;

    //----------------BEST PLAYERS ANN----------------
    ANN *bestLightAssaultANN;
    ANN *bestSniperANN;
    ANN *bestAssaultANN;

    Evolution();
    ~Evolution();

    void createANN(ANN *childs, int inputSize, int outputSize);
    void setPlayersPtr();

    void game();

    //----------------LEARNING RELATIONS----------------
    void tournamentAll();
    void tournament(Player **players, ANN *childs);

    void sintese(ANN *resultANN, ANN *ANN1, ANN *ANN2);
    void crossover(ANN *resultANN, ANN *ANN1, ANN *ANN2);
    void mutation(ANN *ANN);

    void genocideAll();
    void genocide(Player **players);

    scoreData_t setBestIndvs();

    //----------------FILE SYSTEM----------------
    void saveANNAll(const char* fileName);
    void saveANN(ANN* bestMatrix, std::ofstream* fileObj);

    void readANNAll(const char* fileName);
    void readANN(ANN* bestMatrix, std::ifstream* fileObj);

    void reset();
};
