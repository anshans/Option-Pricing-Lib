#pragma once
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <memory>
#include <complex>
#include <math.h>
#include <../DataStructures/OptionStructures.hpp>

class BinomialTree{
public: 
    //Constructor with u and d. Both values are provided as percentage i.e. 
    BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice, float T, int steps);
    //Constructor with volatility
    BinomialTree(float currStockPrice, float vol, float r,
    float strikePrice, float T, int steps);

    float accessParticularOption();
    float calculateEuropean();
    float calculateAmerican();

protected:
    void createPriceTree();
    void calculateOption(OptionType optionType);
    int numInLayer(int layerNum);
    float euContractValue(int layer, int j);
    float earlyExecution(int layer, int j);
private:
    float u;
    float d;
    float r;
    float T;
    int steps;
    float currStockPrice;
    float strikePrice;
    //Memory representation of Binomial tree
    std::unique_ptr<std::unique_ptr<PriceNode[]>[]> layersOfNodes;
};
