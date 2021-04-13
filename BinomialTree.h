#include <stdio.h>
#include <iostream>
#include <ctime>
#include <memory>
#include <complex>

/* simple pricing code based on Binomial trees
    Work to do:
    -Add pricing for many option types and regions,
    -Accelerate using CUDA, since computation is highly pararell
    -
*/


enum OptionType{Call, Put};
enum Region{Am, Eu};

struct PriceNode{
    float stockPrice;
    float contractPrice;
};

class BinomialTree{
public:
    //Tree Constructors
    BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice,
    OptionType option, float T, int steps);
    //ToDo
    float accessParticularOption();
    float calculate();

protected:
    void createPriceTree();
    void calculateOptionPrice();
    float numInLayer(int layerNum);
private:
    float u;
    float d;
    float r;
    float T;
    int steps;
    float currStockPrice;
    //2-dimensional array, 
    std::unique_ptr<std::unique_ptr<PriceNode[]>[]> layersOfNodes;
};
