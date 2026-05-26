#include <cmath>
#include <exception>
#include <iostream>
#include "SquareMatrix.h"



int main(){
    SquareMatrix<int> mat(2);
    mat(0, 0) = 1;
    std::cout<<mat(0,0)<<"\n";
}