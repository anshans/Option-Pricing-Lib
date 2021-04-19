#include <stdio.h>
#include <iostream>
#include <ctime>
#include <memory>
#include <complex>
#include <math.h>   

/* Pricing method based on Binomial trees
    Work to do:
    -Add access to the American.
    -Accelerate using CUDA, computation is highly pararell
*/

struct PriceNode{
    float stockPrice;
    float contractPrice;
};
enum OptionType{Eu,Am};

class BinomialTree{
public: 
    BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice, float T, int steps);
    //Constructor with volatility
    BinomialTree(float currStockPrice, float vol, float r,
    float strikePrice, float T, int steps);

    //ToDo do creator with volatility of stock
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
    //2-dimensional array, 
    std::unique_ptr<std::unique_ptr<PriceNode[]>[]> layersOfNodes;
};
