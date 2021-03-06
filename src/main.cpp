#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include <mutex>

#include "headers/Plot.h"

using namespace std::chrono_literals;

int gen = 1;
std::mutex mtx;
Evolution *evolution;
Plot *plot;

void copyModerator() {

    Moderator *copyModerator = new Moderator;

    LightAssault *lightAssaults = evolution->bestTeams->getLightAssaults();
    Sniper *snipers = evolution->bestTeams->getSnipers();
    Assault *assaults = evolution->bestTeams->getAssaults();

    copyModerator->setScreen(new Screen);
    copyModerator->screen->setScreenParam("best teams", 0, 0);
    copyModerator->setAllPlayersValues();

    Moderator *bestIndvsCopy = new Moderator;

    bestIndvsCopy->setScreen(new Screen);
    bestIndvsCopy->screen->setScreenParam("best indvs", LENGTH + 67, 0);
    bestIndvsCopy->setAllPlayersValues();

    ANN *bestLightAssaultMatrix = new ANN;
    ANN *bestSniperMatrix = new ANN;
    ANN *bestAssaultMatrix = new ANN;

    bestLightAssaultMatrix->setANNParameters(lightAssaults->ANNInputSize, lightAssaults->ANNOutputSize);
    bestSniperMatrix->setANNParameters(snipers->ANNInputSize, snipers->ANNOutputSize);
    bestAssaultMatrix->setANNParameters(assaults->ANNInputSize, assaults->ANNOutputSize);

    while (true) {
        //----------------BEST TEAM MATCH----------------
        mtx.lock();

        copyModerator->setInicialPosAll(initialPos, rand() % 3);
        copyModerator->copyAllWeights(lightAssaults, snipers, assaults);

        mtx.unlock();

        copyModerator->gameOfBest();

        copyModerator->resetAllPlayers(true);
        plot->plotData(gen);

        //----------------BEST PLAYER MATCH----------------
        mtx.lock();
        bestLightAssaultMatrix->copyWheights(evolution->bestLightAssaultANN->getMatrixPtr(), evolution->bestLightAssaultANN->getBiasPtr());
        bestSniperMatrix->copyWheights(evolution->bestSniperANN->getMatrixPtr(), evolution->bestSniperANN->getBiasPtr());
        bestAssaultMatrix->copyWheights(evolution->bestAssaultANN->getMatrixPtr(), evolution->bestAssaultANN->getBiasPtr());

        bestIndvsCopy->setInicialPosAll(initialPos, rand() % 3);
        bestIndvsCopy->setAllWeightsOneMatrix(bestLightAssaultMatrix, bestSniperMatrix, bestAssaultMatrix);
        mtx.unlock();

        bestIndvsCopy->gameOfBest();
        bestIndvsCopy->resetAllPlayers(true);
        //----------------PLOT----------------
        plot->plotData(gen);
    }
}

void showBestGame(){
    std::string filePath;
    std::cout << "Filepath to matrix data:" << std::endl;
    std::cin >> filePath;
    
    //----------------ONE GAME----------------
    evolution->readANNAll(filePath.c_str());
    Moderator *bestIndvsCopy = new Moderator;

    bestIndvsCopy->setScreen(new Screen);
    bestIndvsCopy->screen->setScreenParam("best indvs", LENGTH + 67, 0);
    bestIndvsCopy->setAllPlayersValues();

    ANN *bestLightAssaultMatrix = new ANN;
    ANN *bestSniperMatrix = new ANN;
    ANN *bestAssaultMatrix = new ANN;

    LightAssault *lightAssaults = evolution->bestTeams->getLightAssaults();
    Sniper *snipers = evolution->bestTeams->getSnipers();
    Assault *assaults = evolution->bestTeams->getAssaults();

    bestLightAssaultMatrix->setANNParameters(lightAssaults->ANNInputSize, lightAssaults->ANNOutputSize);
    bestSniperMatrix->setANNParameters(snipers->ANNInputSize, snipers->ANNOutputSize);
    bestAssaultMatrix->setANNParameters(assaults->ANNInputSize, assaults->ANNOutputSize);

    //----------------BEST PLAYER MATCH----------------
    bestLightAssaultMatrix->copyWheights(evolution->bestLightAssaultANN->getMatrixPtr(), evolution->bestLightAssaultANN->getBiasPtr());
    bestSniperMatrix->copyWheights(evolution->bestSniperANN->getMatrixPtr(), evolution->bestSniperANN->getBiasPtr());
    bestAssaultMatrix->copyWheights(evolution->bestAssaultANN->getMatrixPtr(), evolution->bestAssaultANN->getBiasPtr());

    bestIndvsCopy->setInicialPosAll(initialPos, rand() % 3);
    bestIndvsCopy->setAllWeightsOneMatrix(bestLightAssaultMatrix, bestSniperMatrix, bestAssaultMatrix);
    bestIndvsCopy->resetAllPlayers(true);

    while (true) {
        bestIndvsCopy->gameOfBest();
        bestIndvsCopy->setInicialPosAll(initialPos, rand() % 3);
        bestIndvsCopy->resetAllPlayers(true);
    }    
}

int main() {
    srand(time(0));

    evolution = new Evolution;
    scoreData_t scoreData;

    int choose;

    std::cout << "evolution(0) or load file(1)?" << std::endl;
    std::cin >> choose;
    
    if(choose == 1)
        showBestGame();

    std::cout << "save evolution data? n(0) y(1)" << std::endl;
    std::cin >> choose;
    getc(stdin);

    std::string filePath = "";
    
    if(choose){
        std::cout << "directory to save matrix data: (press enter to save on current directory)" << std::endl;
        std::getline (std::cin,filePath);
    }

    plot = new Plot;
    std::thread th(copyModerator);
    
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    long double totalTime = 0;

    while (1) {
        //----------------EVOLUTION----------------
        mtx.lock();

        begin = std::chrono::steady_clock::now();
        evolution->game();
        end = std::chrono::steady_clock::now();
        totalTime += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

        std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0 << "s" << std::endl;
        std::cout << "med time: " << totalTime / (gen * 1000.0) << "s" << std::endl;

        evolution->tournamentAll();

        std::cout << "-------------GEN " << gen << " -------------" << std::endl;
        std::cout << "best light assault team score: " << evolution->bestLightAssaultTeamScore << std::endl;
        std::cout << "best sniper team score: " << evolution->bestSniperTeamScore << std::endl;
        std::cout << "best assault team score: " << evolution->bestAssaultTeamScore << std::endl;

        scoreData = evolution->setBestIndvs();

        if (!(gen % 15)) {
            std::cout << "-------------genocide-------------" << std::endl;
            evolution->genocideAll();
        }

        evolution->reset();

        if(choose){
            if(!(gen % 50)){
                cv::imwrite(filePath + std::to_string(gen) + "gen.png", plot->getGraph());
            }

            if(!(gen % 20)){
                evolution->saveANNAll((filePath + std::to_string(gen) + ".txt").c_str());
            }
        }
        
        plot->addData(scoreData);

        mtx.unlock();

        std::this_thread::sleep_for(1ms);  //time to change threads
        gen++;
    }

    return 0;
}