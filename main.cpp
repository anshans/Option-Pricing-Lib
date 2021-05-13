#include <iostream>
#include <GPU/BinomialTreeGPU.hpp>

//Small tests
int main(){
    BinomialTreeGPU tree{20,1.1,0.9,0.12,21,0.25,3};
    std::cout << "Value: " << tree.calculateEuropean() << std::endl;
    return 0;
}