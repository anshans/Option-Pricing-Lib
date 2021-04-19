#include "BinomialTree.h"


//Constructors
BinomialTree::BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice, float T, int steps):u(u),d(d),currStockPrice(currStockPrice),steps(steps),
    T(T),r(r),strikePrice(strikePrice){
};
BinomialTree::BinomialTree(float currStockPrice, float vol, float r,
    float strikePrice, float T, int steps):currStockPrice(currStockPrice),steps(steps),
    T(T),r(r),strikePrice(strikePrice),u(exp(vol*sqrt(T))),d(-exp(vol*sqrt(T))){
};

void BinomialTree::createPriceTree(){
    //Numer of nodes at beggining of tree
    layersOfNodes = std::make_unique<std::unique_ptr<PriceNode[]>[]>(steps);
    //Create beginning node
    layersOfNodes[0] = std::make_unique<PriceNode[]>(1);
    layersOfNodes[0][0].stockPrice = currStockPrice;

    //Previous layer(parentLayer) is giving values to next layer (parentLayer+1)
    //Last layer, can't have leaves therefore there is steps - 1.
    for(int parentLayer = 0; parentLayer < steps - 1; parentLayer++){
        //Allocate memory for new children!
        layersOfNodes[parentLayer+1] = std::make_unique<PriceNode[]>(numInLayer(parentLayer+1));

        for(int nodeInParentLayer = 0; nodeInParentLayer < numInLayer(parentLayer); nodeInParentLayer++){
            //ineffciency here
            layersOfNodes[parentLayer+1][nodeInParentLayer].stockPrice = layersOfNodes[parentLayer][nodeInParentLayer].stockPrice * u;
            layersOfNodes[parentLayer+1][nodeInParentLayer+1].stockPrice = layersOfNodes[parentLayer][nodeInParentLayer].stockPrice * d;
        }
    } 
};
float BinomialTree::calculateEuropean(){
    //Allocate space in memory to keep all of the nodes  
    createPriceTree();
    calculateOption(Eu);
    return layersOfNodes[0][0].contractPrice;
};
float BinomialTree::calculateAmerican(){
    createPriceTree();
    calculateOption(Am);
    return layersOfNodes[0][0].contractPrice;
};

int BinomialTree::numInLayer(int layerNum){
    //layerNum counts from 0 just like in unique_ptr array!
    return (layerNum+1);
};

void BinomialTree::calculateOption(OptionType optionType){

    int numOfNodes;
    for(int layer=steps-1; layer >=0; layer--){
        numOfNodes = numInLayer(layer);

        if(layer<steps-1){

            for(int j = 0; j < numOfNodes; j++){
                    float contractValue=euContractValue(layer, j);
                    if(optionType == OptionType::Eu){
                        layersOfNodes[layer][j].contractPrice=contractValue;
                    }
                    else if (optionType == OptionType::Am){
                        layersOfNodes[layer][j].contractPrice=(layersOfNodes[layer][j].stockPrice - strikePrice > contractValue)
                        ?layersOfNodes[layer][j].stockPrice - strikePrice:contractValue;
                    }
            }
        }
        //It's option value at expiration date.
        else{
            for(int j = 0; j<numOfNodes; j++){
                float optionValue = layersOfNodes[layer][j].stockPrice-strikePrice;
                layersOfNodes[layer][j].contractPrice = ((optionValue>0)?(optionValue):0);
            }  
        }
    }
    
};
float BinomialTree::euContractValue(int layer, int j){
    //For nodes deeper within the tree, equation 13.2 from Hull book
    //is being used.
    float p = (exp(r*T)-d) / (u-d);
    float fu =layersOfNodes[layer+1][j].contractPrice;
    float fd =layersOfNodes[layer+1][j+1].contractPrice;
    //13.2 Hull
    return exp(-r*T)*(p*fu+(1-p)*fd);
}

int main(){
    float currVal = BinomialTree(20,1.1,0.9,0.12,21,0.25,3).calculateEuropean();
    std::cout<<"Result: "<<currVal<<"\n";
    return 0;
};