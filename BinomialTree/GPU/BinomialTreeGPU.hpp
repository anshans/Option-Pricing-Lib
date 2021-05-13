#pragma once
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <memory>
#include <complex>
#include <math.h>
#include <../DataStructures/OptionStructures.hpp>

//GPU functions
using LayerCalculation = int (*)(int);
using ContractCalculation = float (*)(PriceNode **, int, int, float, float, float);

//Classes
class BinomialTreeGPU
{
public:
    BinomialTreeGPU(float currStockPrice, float u, float d, float r,
                 float strikePrice, float T, int steps);
    //Constructor with volatility
    BinomialTreeGPU(float currStockPrice, float vol, float r,
                 float strikePrice, float T, int steps);
    //Destructor

    float calculateEuropean();
    float calculateAmerican();
    static int numInLayer(int layerNum);
    static float euContractValue(PriceNode** layersOfNodes, int layer, int j, float p, float T);

protected:
    void createPriceTree();
    void calculateOption(OptionType optionType);
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
    PriceNode **layersOfNodes;
};

//GPU kernels
__global__ void fillPriceTree(PriceNode **layersOfNodes, int steps, float u, float d, LayerCalculation layerFunction);
__global__ void calculateContractLayer(PriceNode **layersOfNodes, int layer, float p,
                                       float strikePrice, int numOfNodes, float T, float r, OptionType optionType, ContractCalculation euContractValue);