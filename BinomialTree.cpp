#include "BinomialTree.h"

void BinomialTree::createPriceTree(float currStockPrice){
    //Create layer array
    layersOfNodes= std::make_unique<std::unique_ptr<PriceNode[]>[]>(steps);
    //Numer of nodes at beggining of tree
    int numOfNodes = 1;
    layersOfNodes[0] = std::make_unique<PriceNode[]>(numOfNodes);
    layersOfNodes[0][0].stockPrice = currStockPrice;

    //ParentLayer is a layer that is giving values for next nodes in the tree.
    //Last layer, can't have leaves since there is steps - 1.
    for(int parentLayer = 0; parentLayer < steps - 1; parentLayer++){
        
        //Allocate memory for new children!
        layersOfNodes[parentLayer+1] = std::make_unique<PriceNode[]>(numInLayer(parentLayer+1));
      
        //Each node in layer
        for(int nodeInLayer = 0; nodeInLayer < nodeInLayer; nodeInLayer++){
            layersOfNodes[parentLayer+1][2*nodeInLayer].stockPrice 
            = layersOfNodes[parentLayer][nodeInLayer].stockPrice * u;
            
            layersOfNodes[parentLayer+1][2*nodeInLayer+1].stockPrice 
            = layersOfNodes[parentLayer][nodeInLayer].stockPrice * d;
        }
    }
};

BinomialTree::BinomialTree(float currStockPrice, float u, float d, float r,
    float strikePrice,
    OptionType option, float T, int steps):u(u),d(d),currStockPrice(currStockPrice),steps(steps),
    T(T),r(r){
        //In case of extending add Option to the CTOR
    
};
float BinomialTree::calculate(){
    //Allocate space in memory to keep all of the nodes  
    createPriceTree(currStockPrice);
    return layersOfNodes[0][0].contractPrice;
}
float BinomialTree::numInLayer(int layerNum){
    return 2^(layerNum);
}

void BinomialTree::calculateOptionPrice(){
//Actually pricing is only far calls EU
//TO DO
    int numOfNodes;
    for(int layer=steps-1; layer >=0; layer--){
        numOfNodes = numInLayer(layer);
        if(layer!=steps-1){
            //For nodes deeper within the tree, equation 13.2 from Hull book
            //is being used.
            for(int j = 0; j < numOfNodes; j++){
                float p = (exp(-r*T)-d) / (u-d);
                float fu =layersOfNodes[layer+1][j*2].contractPrice;
                float fd =layersOfNodes[layer+1][j*2+1].contractPrice;
                //13.2
                layersOfNodes[layer][j].contractPrice=exp(r*T)*(p*fu+(1-p)*fd);
            }
        }
        
        //DO FOR OTHER CONTRACTS
        //It's option value at expiration date.
        else{

            for(int j = 0; j<numOfNodes; j++){
                layersOfNodes[layer][j].contractPrice = 
                (layersOfNodes[layer][j].stockPrice > layersOfNodes[layer][j].contractPrice) 
                ? layersOfNodes[layer][j].stockPrice-layersOfNodes[layer][j].contractPrice 
                : 0;
            }  
        }
    }
}

int main(){
    float currVal = BinomialTree(100,1.01,0.99,0.1,120,Call,1,2).calculate();
    std::cout<<currVal;
    return 0;
}