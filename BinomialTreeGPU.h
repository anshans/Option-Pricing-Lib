#include <stdio.h>
#include <iostream>
#include <ctime>
#include <memory>
#include <complex>
#include <math.h>   

/* Pricing method based on Binomial trees */
//Data structures
struct PriceNode{
    float stockPrice;
    float contractPrice;
};
enum OptionType{Eu,Am};

//GPU functions
__global__ void fillPriceTree(PriceNode** layersOfNodes, int steps, float u, float d);

//Classes
class BinomialTree{
public: 
    BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice, float T, int steps);
    //Constructor with volatility
    BinomialTree(float currStockPrice, float vol, float r,
    float strikePrice, float T, int steps);
    //Destructor

    float calculateEuropean();
    float calculateAmerican();
    static int numInLayer(int layerNum);

protected:
    void createPriceTree();
    void calculateOption(OptionType optionType);
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
    PriceNode** layersOfNodes;
};
