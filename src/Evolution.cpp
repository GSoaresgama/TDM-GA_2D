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

    //----------------TEMP INDIVS----------------
    Moderator tempMod;
    tempMod.setScreen(new Screen);
    tempMod.setAllPlayersValues();

    //----------------BEST INDIVS ANN----------------
    bestLightAssaultANN = new ANN;
    bestSniperANN = new ANN;
    bestAssaultANN = new ANN;

    bestLightAssaultANN->setANNParameters(tempMod.getLightAssaults()->ANNInputSize, tempMod.getLightAssaults()->ANNOutputSize);
    bestSniperANN->setANNParameters(tempMod.getSnipers()->ANNInputSize, tempMod.getSnipers()->ANNOutputSize);
    bestAssaultANN->setANNParameters(tempMod.getAssaults()->ANNInputSize, tempMod.getAssaults()->ANNOutputSize);

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

    int posIndex = rand() % 3;

    //----------------GAME----------------

    for (i = 0; i < POP_SIZE; i++)
        lightAssaultTraining[i].game();

    for (i = 0; i < POP_SIZE; i++)
        snipersTraining[i].game();

    for (i = 0; i < POP_SIZE; i++)
        assaultsTraining[i].game();


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

    ANN *currentANN;
    ANN *ANNBest1;
    ANN *ANNBest2;

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
    for (i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++) {
        if (i == bestIndex || i == secondBestIndex || i == worstIndex)
            continue;

        currentANN = &childs[i];

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

        ANNBest1 = best1->ann;
        ANNBest2 = best2->ann;

        crossover(currentANN, ANNBest1, ANNBest2);
        mutation(currentANN);
    }

    //----------------SET MATRIX----------------
    for (i = 0; i < TOTAL_NUMBER_OF_PLAYERS; i++) {
        if (i == bestIndex || i == secondBestIndex)
            continue;

        if (i == worstIndex) {
            currentANN = &childs[worstIndex];
            ANNBest1 = players[bestIndex]->ann;
            ANNBest2 = players[secondBestIndex]->ann;
            sintese(currentANN, ANNBest1, ANNBest2);
            mutation(currentANN);
        }

        //changes ptr
        childs[i].setBias(players[i]->ann->setBias(childs[i].getBiasPtr()));
        childs[i].setMatrix(players[i]->ann->setMatrix(childs[i].getMatrixPtr()));
    }
}
void Evolution::sintese(ANN *resultANN, ANN *ANN1, ANN *ANN2) {

    int layer;
    int totalSize;
    int count;

    vectorF *resultBias = resultANN->getBiasPtr();
    vectorF *bias1 = ANN1->getBiasPtr();
    vectorF *bias2 = ANN2->getBiasPtr();

    MatrixF *resultMatrix = resultANN->getMatrixPtr();
    MatrixF *matrix1 = ANN1->getMatrixPtr();
    MatrixF *matrix2 = ANN2->getMatrixPtr();

    for ( layer = 0; layer < layerSize + 1; layer++) {
        
        totalSize = matrix1[layerSize].lines*matrix1[layer].coluns;
        for (count = 0; count < totalSize; count++) {
            resultMatrix[layer].matrix[count] = (matrix1[layer][count] + matrix2[layer][count]) / 2.0;
        }

        for (count = 0; count < bias1[layer].size; count++) {
            resultBias[layer].vector[count] = (bias1[layer][count] + bias2[layer][count]) / 2.0;
        }
    }
    
    
}

void Evolution::crossover(ANN *resultANN, ANN *ANN1, ANN *ANN2) {
    int crossChance = 5;  // 0.1% * crossChance

    int layer;
    int posi;
    int side;
    int totalSize;

    vectorF *resultBias = resultANN->getBiasPtr();
    vectorF *bias1 = ANN1->getBiasPtr();
    vectorF *bias2 = ANN2->getBiasPtr();

    MatrixF *resultMatrix = resultANN->getMatrixPtr();
    MatrixF *matrix1 = ANN1->getMatrixPtr();
    MatrixF *matrix2 = ANN2->getMatrixPtr();

    for (layer = 0; layer < layerSize + 1; layer++) {
        totalSize = bias1->size;
        side = 0;

        for (posi = 0; posi < totalSize; posi++) {
            if (side == 1){
                resultBias[layer].vector[posi] = bias2[layer][posi];
                    if (rand() % 1000 < crossChance)
                        side = 0;
            } else {
                resultBias[layer].vector[posi] = bias1[layer][posi];
                if (rand() % 1000 < crossChance)
                    side = 1;
            }
        }

        totalSize = matrix1[layer].lines * matrix1[layer].coluns;
        side = 0;

        for (posi = 0; posi < totalSize; posi++) {
            if (side == 1){
                resultMatrix[layer].matrix[posi] = matrix2[layer][posi];
                    if (rand() % 1000 < crossChance)
                        side = 0;
            } else {
                resultMatrix[layer].matrix[posi] = matrix1[layer][posi];
                if (rand() % 1000 < crossChance)
                    side = 1;
            }
        }
    }
}

void Evolution::mutation(ANN *ANN) {
    int quant;
    int maxMut;

    int posi;
    int maxPosi;

    vectorF* biasArray = ANN->getBiasPtr();
    MatrixF* matrixArray = ANN->getMatrixPtr();

    for (int i = 0; i < layerSize + 1; i++) {
        maxMut = rand() % 4 + 1;  // 1 - 5
        maxPosi = biasArray[i].size;
        
        for (quant = 0; quant < maxMut; quant++) {
            posi = rand() % maxPosi;
            biasArray[i].vector[posi] += (rand() % (2 * 750) - 750) / 1000.0;
        }
        
        maxMut = rand() % 26 + 4;  // 4-30
        maxPosi = matrixArray[i].lines * matrixArray[i].coluns;
        
        for (quant = 0; quant < maxMut; quant++) {
            posi = rand() % maxPosi;
            matrixArray[i].matrix[posi] += (rand() % (2 * 750) - 750) / 1000.0;
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
            players[j]->ann->setBias(players[i]->ann->setBias(players[j]->ann->getBiasPtr()));
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
        MLAS += lightAssaultTraining[i].lightAssaultScoreSum;
        MSS += snipersTraining[i].sniperScoreSum;
        MAS += assaultsTraining[i].assaultScoreSum;

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

    std::cout << "mediun light assault score: " << MLAS << std::endl;
    std::cout << "mediun sniper score: " << MSS << std::endl;
    std::cout << "mediun assault score: " << MAS << std::endl;

    //----------------SET WEIGHTS----------------
    Player *BLA = lightAssaultTraining[BLAI].bestLightAssault->player;
    Player *BS = snipersTraining[BSI].bestSniper->player;
    Player *BA = assaultsTraining[BAI].bestAssault->player;

    bestLightAssaultANN->copyWheights(BLA->ann->getMatrixPtr(), BLA->ann->getBiasPtr());
    bestSniperANN->copyWheights(BS->ann->getMatrixPtr(), BS->ann->getBiasPtr());
    bestAssaultANN->copyWheights(BA->ann->getMatrixPtr(), BA->ann->getBiasPtr());
    
    return {BLAS, BSS, BAS, MLAS, MSS, MAS};
}

void Evolution::saveANNAll(const char* fileName){
    std::ofstream fileObj(fileName);
    
    if(!fileObj.is_open()){
        std::cout << "can't save on file " << fileName << std::endl;
        exit(1);
    }

    saveANN(bestLightAssaultANN, &fileObj);
    saveANN(bestSniperANN, &fileObj);
    saveANN(bestAssaultANN, &fileObj);
    fileObj.close();
}

void Evolution::saveANN(ANN* bestANN, std::ofstream* fileObj) {

    vectorF *biasPtr;
    MatrixF *matrixPtr;

    biasPtr = bestANN->getBiasPtr();
    matrixPtr = bestANN->getMatrixPtr();

    for (int i = 0; i < layerSize + 1; i++) {
        biasPtr[i].writeVectorToFile(fileObj);
        matrixPtr[i].writeMatrixToFile(fileObj);
    }
}

void Evolution::readANNAll(const char* fileName){
    std::ifstream fileObj;
    fileObj.open(fileName, std::ios::in);
    
    if(!fileObj.is_open()){
        std::cout << "can't read from file " << fileName << std::endl;
        exit(1);
    }
    
    readANN(bestLightAssaultANN, &fileObj);
    readANN(bestSniperANN, &fileObj);
    readANN(bestAssaultANN, &fileObj);
}

void Evolution::readANN(ANN* bestANN, std::ifstream* fileObj){
    vectorF *biasPtr;
    MatrixF *matrixPtr;

    biasPtr = bestANN->getBiasPtr();
    matrixPtr = bestANN->getMatrixPtr();

    for (int i = 0; i < layerSize + 1; i++) {
        biasPtr[i].readVectorFromFile(fileObj);
        matrixPtr[i].readMatrixFromFile(fileObj);
    }
}
