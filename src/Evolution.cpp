#include <eigen3/Eigen/Dense>
#include <iostream>
#include <omp.h>
#include <time.h>

#include "headers/Evolution.h"

using namespace Eigen;

Evolution::Evolution() {
    //----------------BEST SCORES----------------
    bestLightAssaultTeamScore = INICIAL_SCORE;
    bestSniperTeamScore = INICIAL_SCORE;
    bestAssaultTeamScore = INICIAL_SCORE;

    //----------------BEST INDIVS----------------
    bestIndvs = new Moderator;
    bestIndvs->setScreen(new Screen);
    bestIndvs->setAllPlayersValues();

    //----------------BEST INDIVS ANN----------------
    bestLightAssaultANN = new ANN;
    bestSniperANN = new ANN;
    bestAssaultANN = new ANN;

    bestLightAssaultANN->setANNParameters(bestIndvs->getLightAssaults()->ANNInputSize, bestIndvs->getLightAssaults()->ANNOutputSize);
    bestSniperANN->setANNParameters(bestIndvs->getSnipers()->ANNInputSize, bestIndvs->getSnipers()->ANNOutputSize);
    bestAssaultANN->setANNParameters(bestIndvs->getAssaults()->ANNInputSize, bestIndvs->getAssaults()->ANNOutputSize);

    //----------------BEST TEAMS----------------
    bestTeams = new Moderator;
    bestTeams->setScreen(new Screen);
    bestTeams->setAllPlayersValues();

    TOTAL_NUMBER_OF_PLAYERS = NUMBER_OF_PLAYERS * POP_SIZE;

    //----------------TRAINING MODERATOR----------------
    lightAssaultTraining = new Moderator[POP_SIZE];
    snipersTraining = new Moderator[POP_SIZE];
    assaultsTraining = new Moderator[POP_SIZE];

    //----------------SETTING PARAMS----------------
    for (int i = 0; i < POP_SIZE; i++) {
        //----------------LIGHT ASSAULT----------------
        lightAssaultTraining[i].setScreen(new Screen);
        lightAssaultTraining[i].setAllPlayersValues();
        lightAssaultTraining[i].setAllWeights(nullptr, bestTeams->getSnipers(), bestTeams->getAssaults());

        //----------------SNIPER----------------
        snipersTraining[i].setScreen(new Screen);
        snipersTraining[i].setAllPlayersValues();
        snipersTraining[i].setAllWeights(bestTeams->getLightAssaults(), nullptr, bestTeams->getAssaults());

        //----------------ASSAULT----------------
        assaultsTraining[i].setScreen(new Screen);
        assaultsTraining[i].setAllPlayersValues();
        assaultsTraining[i].setAllWeights(bestTeams->getLightAssaults(), bestTeams->getSnipers(), nullptr);
    }

    //----------------CHILDS----------------
    lightAssaultChilds = new ANN[TOTAL_NUMBER_OF_PLAYERS];
    snipersChilds = new ANN[TOTAL_NUMBER_OF_PLAYERS];
    assaultsChilds = new ANN[TOTAL_NUMBER_OF_PLAYERS];

    createANN(lightAssaultChilds, bestTeams->getLightAssaults()->ANNInputSize, bestTeams->getLightAssaults()->ANNOutputSize);
    createANN(snipersChilds, bestTeams->getSnipers()->ANNInputSize, bestTeams->getSnipers()->ANNOutputSize);
    createANN(assaultsChilds, bestTeams->getAssaults()->ANNInputSize, bestTeams->getAssaults()->ANNOutputSize);

    //----------------ALL PLAYERS----------------
    allLightAssaults = new Player *[TOTAL_NUMBER_OF_PLAYERS];
    allSnipers = new Player *[TOTAL_NUMBER_OF_PLAYERS];
    allAssaults = new Player *[TOTAL_NUMBER_OF_PLAYERS];

    setPlayersPtr();
}

Evolution::~Evolution() {
}

void Evolution::createANN(ANN *childs, int inputSize, int outputSize) {
    for (int i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++)
        childs[i].setANNParameters(inputSize, outputSize);
}

void Evolution::setPlayersPtr() {
    int i;
    int j;

    int numberOfLightAssaults = 0;
    int numberOfSnipers = 0;
    int numberOfAssaults = 0;

    Player *playersPtr;

    //----------------LIGHT ASSAULTS----------------
    for (i = 0; i < POP_SIZE; i++) {
        playersPtr = lightAssaultTraining[i].getLightAssaults();
        for (j = 0; j < NUMBER_OF_PLAYERS; j++) {
            allLightAssaults[numberOfLightAssaults + j] = &playersPtr[j];
        }

        numberOfLightAssaults += NUMBER_OF_PLAYERS;
    }

    //----------------SNIPERS----------------
    for (i = 0; i < POP_SIZE; i++) {
        playersPtr = snipersTraining[i].getSnipers();

        for (j = 0; j < NUMBER_OF_PLAYERS; j++) {
            allSnipers[numberOfSnipers + j] = &playersPtr[j];
        }

        numberOfSnipers += NUMBER_OF_PLAYERS;
    }

    //----------------ASSAULTS----------------
    for (i = 0; i < POP_SIZE; i++) {
        playersPtr = assaultsTraining[i].getAssaults();

        for (j = 0; j < NUMBER_OF_PLAYERS; j++) {
            allAssaults[numberOfAssaults + j] = &playersPtr[j];
        }
        numberOfAssaults += NUMBER_OF_PLAYERS;
    }
}

void Evolution::game() {
    int i;

    //----------------GAME----------------
#pragma omp parallel for
    for (i = 0; i < POP_SIZE; i++) {
        lightAssaultTraining[i].game();
        snipersTraining[i].game();
        assaultsTraining[i].game();
    }

    //----------------SELECT BEST TEAMS----------------
    for (i = 0; i < POP_SIZE; i++) {
        lightAssaultTraining[i].calculateScore();
        snipersTraining[i].calculateScore();
        assaultsTraining[i].calculateScore();

        if (lightAssaultTraining[i].lightAssaultScore > bestLightAssaultTeamScore) {
            bestLightAssaultTeamScore = lightAssaultTraining[i].lightAssaultScore;
            bestLightAssaults = &lightAssaultTraining[i];
        }

        if (snipersTraining[i].sniperScore > bestSniperTeamScore) {
            bestSniperTeamScore = snipersTraining[i].sniperScore;
            bestSnipers = &snipersTraining[i];
        }

        if (assaultsTraining[i].assaultScore > bestAssaultTeamScore) {
            bestAssaultTeamScore = assaultsTraining[i].assaultScore;
            bestAssaults = &assaultsTraining[i];
        }
    }
}

void Evolution::reset() {
    bestLightAssaultTeamScore = INICIAL_SCORE;
    bestSniperTeamScore = INICIAL_SCORE;
    bestAssaultTeamScore = INICIAL_SCORE;

    //----------------SET BEST TEAMS----------------
    bestTeams->setAllWeights(bestLightAssaults->getLightAssaults(), bestSnipers->getSnipers(), bestAssaults->getAssaults());

    // cv::Point initialPos[] = {cv::Point(LENGTH - 300, HEIGHT - 250), cv::Point(LENGTH - 400, 150), cv::Point(0, 150)};
    int posIndex = rand() % 3;

    //----------------RESET MODERATORS----------------
#pragma omp parallel for
    for (int i = 0; i < POP_SIZE; i++) {
        //----------------INICIAL POSITION----------------
        lightAssaultTraining[i].setInicialPosAll(initialPos, posIndex);
        snipersTraining[i].setInicialPosAll(initialPos, posIndex);
        assaultsTraining[i].setInicialPosAll(initialPos, posIndex);

        //----------------WEIGHTS----------------
        lightAssaultTraining[i].copyAllWeights(nullptr, bestTeams->getSnipers(), bestTeams->getAssaults());
        snipersTraining[i].copyAllWeights(bestTeams->getLightAssaults(), nullptr, bestTeams->getAssaults());
        assaultsTraining[i].copyAllWeights(bestTeams->getLightAssaults(), bestTeams->getSnipers(), nullptr);

        //----------------RESET PLAYERS----------------
        lightAssaultTraining[i].resetAllPlayers(true);
        snipersTraining[i].resetAllPlayers(true);
        assaultsTraining[i].resetAllPlayers(true);
    }

    bestTeams->resetAllPlayers(true);
}

void Evolution::tournamentAll() {
    tournament(allLightAssaults, lightAssaultChilds);
    tournament(allSnipers, snipersChilds);
    tournament(allAssaults, assaultsChilds);
}

void Evolution::tournament(Player **players, ANN *childs) {
    int i;
    int bestIndex = 0;
    int secondBestIndex = 0;
    int worstIndex = 0;

    int j;

    float bestScore = INICIAL_SCORE;
    float secondBestScore = INICIAL_SCORE;
    float worstScore = -INICIAL_SCORE;

    Player *father1;
    Player *father2;

    Player *best1;
    Player *best2;

    MatrixXf *matrixArray;
    MatrixXf *matrixArrayBest1;
    MatrixXf *matrixArrayBest2;

    //----------------GET TOP 2 && WORST----------------
    for (i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++) {
        if (players[i]->getScore() > bestScore) {
            secondBestScore = bestScore;
            secondBestIndex = i;

            bestScore = players[i]->getScore();
            bestIndex = i;

        } else if (players[i]->getScore() > secondBestScore) {
            secondBestScore = players[i]->getScore();
            secondBestIndex = i;
        }

        if (players[i]->getScore() < worstScore) {
            worstScore = players[i]->getScore();
            worstIndex = i;
        }
    }

    //----------------TOURNAMENT----------------
#pragma omp parallel for
    for (i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++) {
        if (i == bestIndex || i == secondBestIndex)
            continue;

        matrixArray = childs[i].getMatrixPtr();

        father1 = players[rand() % TOTAL_NUMBER_OF_PLAYERS];

        for (j = 0; j < TOURNAMENT_K; j++) {
            father2 = players[rand() % TOTAL_NUMBER_OF_PLAYERS];
            if (father2->getScore() > father1->getScore())
                father1 = father2;
        }
        best1 = father1;

        father1 = players[rand() % TOTAL_NUMBER_OF_PLAYERS];

        for (j = 0; j < TOURNAMENT_K; j++) {
            father2 = players[rand() % TOTAL_NUMBER_OF_PLAYERS];
            if (father2->getScore() > father1->getScore())
                father1 = father2;
        }
        best2 = father1;

        matrixArrayBest1 = best1->ann->getMatrixPtr();
        matrixArrayBest2 = best2->ann->getMatrixPtr();

        for (j = 0; j < layerSize + 1; j++) {
            matrixArray[j] = (matrixArrayBest1[j] + matrixArrayBest2[j]) / 2.0;
        }

        mutation(matrixArray);
    }

    //----------------SET MATRIX----------------
#pragma omp parallel for
    for (i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++) {
        if (i == bestIndex || i == secondBestIndex)
            continue;

        if (i == worstIndex) {
            matrixArray = childs[worstIndex].getMatrixPtr();
            matrixArrayBest1 = players[bestIndex]->ann->getMatrixPtr();
            matrixArrayBest2 = players[secondBestIndex]->ann->getMatrixPtr();

            for (j = 0; j < layerSize + 1; j++) {
                matrixArray[j] = (matrixArrayBest1[j] + matrixArrayBest2[j]) / 2.0;
            }
        }

        //changes ptr
        childs[i].setMatrix(players[i]->ann->setMatrix(childs[i].getMatrixPtr()));
    }
}

void Evolution::mutation(MatrixXf *matrixArray) {
    int quant;
    int line;
    int colun;
    int maxMut;

    for (int i = 0; i < layerSize + 1; i++) {
        maxMut = rand() % 35 + 5;  // 5-40
        for (quant = 0; quant < maxMut; quant++) {
            line = rand() % matrixArray[i].rows();
            colun = rand() % matrixArray[i].cols();

            matrixArray[i](line, colun) += (rand() % (2 * 750) - 750) / 1000.0;
        }
    }
}

void quickSort(Player **players, int low, int high) {
    int i = low;
    int j = high;
    float pivot = players[(i + j) / 2]->getScore();
    float temp;

    while (i <= j) {
        while (players[i]->getScore() < pivot)
            i++;

        while (players[j]->getScore() > pivot)
            j--;

        if (i <= j) {
            //swap
            players[j]->ann->setMatrix(players[i]->ann->setMatrix(players[j]->ann->getMatrixPtr()));

            temp = players[j]->getScore();
            players[j]->setScore(players[i]->getScore());
            players[i]->setScore(temp);

            i++;
            j--;
        }
    }

    if (j > low)
        quickSort(players, low, j);
    if (i < high)
        quickSort(players, i, high);
}

void Evolution::genocideAll() {
    genocide(allLightAssaults);
    genocide(allSnipers);
    genocide(allAssaults);
}

void Evolution::genocide(Player **players) {
    quickSort(players, 0, NUMBER_OF_PLAYERS - 1);

    for (int i = 0; i < NUMBER_OF_PLAYERS / 6; i++) {
        players[i]->ann->reset();
    }
}

scoreData_t Evolution::setBestIndvs() {
    float BLAS = INICIAL_SCORE;
    int BLAI = 0;

    float BSS = INICIAL_SCORE;
    int BSI = 0;

    float BAS = INICIAL_SCORE;
    int BAI = 0;

    float MLAS = 0;
    float MSS = 0;
    float MAS = 0;

    //----------------SET BEST INDV PLAYERS----------------
    for (int i = 0; i < POP_SIZE; i++) {
        MLAS += lightAssaultTraining[i].bestLightAssault->score;
        MSS += snipersTraining[i].bestSniper->score;
        MAS += assaultsTraining[i].bestAssault->score;

        if (lightAssaultTraining[i].bestLightAssault->score > BLAS) {
            BLAS = lightAssaultTraining[i].bestLightAssault->score;
            BLAI = i;
        }

        if (assaultsTraining[i].bestAssault->score > BAS) {
            BAS = assaultsTraining[i].bestAssault->score;
            BAI = i;
        }

        if (snipersTraining[i].bestSniper->score > BSS) {
            BSS = snipersTraining[i].bestSniper->score;
            BSI = i;
        }
    }

    MLAS /= (float)TOTAL_NUMBER_OF_PLAYERS;
    MSS /= (float)TOTAL_NUMBER_OF_PLAYERS;
    MAS /= (float)TOTAL_NUMBER_OF_PLAYERS;

    std::cout << "best light assault score: " << BLAS << std::endl;
    std::cout << "best sniper score: " << BSS << std::endl;
    std::cout << "best assault score: " << BAS << std::endl;

    // std::cout << "mediun light assault score: " << MLAS << std::endl;
    // std::cout << "mediun sniper score: " << MSS << std::endl;
    // std::cout << "mediun assault score: " << MAS << std::endl;


    //----------------SET WEIGHTS----------------
    bestLightAssaultANN->copyWheights(lightAssaultTraining[BLAI].bestLightAssault->player->ann->getMatrixPtr());
    bestSniperANN->copyWheights(snipersTraining[BSI].bestSniper->player->ann->getMatrixPtr());
    bestAssaultANN->copyWheights(assaultsTraining[BAI].bestAssault->player->ann->getMatrixPtr());

    bestIndvs->setAllWeightsOneMatrix(bestLightAssaultANN->getMatrixPtr(), bestSniperANN->getMatrixPtr(), bestAssaultANN->getMatrixPtr());

    return {BLAS, BSS, BAS, MLAS, MSS, MAS};
}