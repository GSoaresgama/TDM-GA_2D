#pragma once

#include "Evolution.h"

#define GRAPH_WIDTH 1200
#define GRAPH_HEIGHT 300

class Plot
{
private:
    float max;
    float min;
    
    cv::Mat graph;
    std::vector<scoreData_t> data;

public:
    Plot();
    ~Plot();

    cv::Mat getGraph();
    void addData(scoreData_t data);
    void plotData(int gen);
};
