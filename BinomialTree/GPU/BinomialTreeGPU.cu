#include "BinomialTreeGPU.hpp"
//Constructors
BinomialTreeGPU::BinomialTreeGPU(float currStockPrice, float u, float d, float r,
                           float strikePrice, float T, int steps) : u(u), d(d), currStockPrice(currStockPrice), steps(steps),
                                                                    T(T), r(r), strikePrice(strikePrice){};
BinomialTreeGPU::BinomialTreeGPU(float currStockPrice, float vol, float r,
                           float strikePrice, float T, int steps) : currStockPrice(currStockPrice), steps(steps),
                                                                    T(T), r(r), strikePrice(strikePrice), u(exp(vol * sqrt(T))), d(-exp(vol * sqrt(T))){};

// Creation of pricce tree
void BinomialTreeGPU::createPriceTree()
{
    //Numer of nodes at beggining of tree
    //layersOfNodes = std::make_unique<std::unique_ptr<PriceNode[]>[]>(steps);
    cudaMallocManaged(&layersOfNodes, steps * sizeof(PriceNode **));
    //Create beginning node
    layersOfNodes[0] = new PriceNode();
    layersOfNodes[0][0].stockPrice = currStockPrice;
    //Previous layer(parentLayer) is giving values to next layer (parentLayer+1)
    //Last layer, can't have leaves therefore there is steps - 1.
    fillPriceTree<<<1, 8>>>(layersOfNodes, steps, u, d, &BinomialTreeGPU::numInLayer);
};

__global__ void fillPriceTree(PriceNode **layersOfNodes, int steps, float u, float d, LayerCalculation layerFunction)
{
    //To get better understanding what is going here, check "An Even Easier Introduction to CUDA"
    int index = threadIdx.x;
    int stride = blockDim.x;
    //Allocate memory for nodes and calculate stock prices for each node.
    for (int parentLayer = 0; parentLayer < steps - 1; parentLayer++)
    {
        layersOfNodes[parentLayer + 1] = new PriceNode[layerFunction(parentLayer + 1)];

        for (int nodeInParentLayer = index; nodeInParentLayer < layerFunction(parentLayer); nodeInParentLayer += stride)
        {
            layersOfNodes[parentLayer + 1][nodeInParentLayer].stockPrice = layersOfNodes[parentLayer][nodeInParentLayer].stockPrice * u;
            layersOfNodes[parentLayer + 1][nodeInParentLayer + 1].stockPrice = layersOfNodes[parentLayer][nodeInParentLayer].stockPrice * d;
        }
    }
};

//Calculation of derivative price.
void BinomialTreeGPU::calculateOption(OptionType optionType)
{
    int numOfNodes;
    float p = (exp(r * T) - d) / (u - d);

    for (int layer = steps - 1; layer >= 0; layer--)
    {
        numOfNodes = numInLayer(layer);

        if (layer < steps - 1)
        {
            calculateContractLayer<<<1,8>>>(layersOfNodes, layer, p, strikePrice, numOfNodes, T, r, optionType, BinomialTreeGPU::euContractValue);
        }
        //It's option value at expiration date.
        else
        {
            for (int j = 0; j < numOfNodes; j++)
            {
                float optionValue = layersOfNodes[layer][j].stockPrice - strikePrice;
                layersOfNodes[layer][j].contractPrice = ((optionValue > 0) ? (optionValue) : 0);
            }
        }
    }
};
__global__ void calculateContractLayer(PriceNode **layersOfNodes, int layer, float p, float strikePrice, int numOfNodes, float T, float r, OptionType optionType, ContractCalculation euContractValue)
{
    int index = threadIdx.x;
    int stride = blockDim.x;
    for (int j = index; j < numOfNodes; j+=stride)
    {
        float contractValue = euContractValue(layersOfNodes, layer, j, p, T, r);
        if (optionType == OptionType::Eu)
        {
            layersOfNodes[layer][j].contractPrice = contractValue;
        }
        else if (optionType == OptionType::Am)
        {
            layersOfNodes[layer][j].contractPrice = (layersOfNodes[layer][j].stockPrice - strikePrice > contractValue)
                                                        ? layersOfNodes[layer][j].stockPrice - strikePrice
                                                        : contractValue;
        }
    }
}

float BinomialTreeGPU::euContractValue(PriceNode** layersOfNodes, int layer, int j, float p, float T, float r)
{
    //For nodes deeper within the tree, equation 13.2 from Hull book
    //is being used.
    float fu = layersOfNodes[layer + 1][j].contractPrice;
    float fd = layersOfNodes[layer + 1][j + 1].contractPrice;
    //13.2 Hull
    return __exp(-r * T) * (p * fu + (1 - p) * fd);
};

float BinomialTreeGPU::calculateEuropean()
{
    createPriceTree();
    calculateOption(Eu);
    return layersOfNodes[0][0].contractPrice;
};
float BinomialTreeGPU::calculateAmerican()
{
    createPriceTree();
    calculateOption(Am);
    return layersOfNodes[0][0].contractPrice;
};
int BinomialTreeGPU::numInLayer(int layerNum)
{
    //layerNum counts from 0 just like in unique_ptr array!
    return (layerNum + 1);
};