#include <gtest/gtest.h>
#include <cmath>
#include "SquareMatrix.h"

TEST(SquareMatrixTest, AddAndScale){
    int dataA[] = {1, 2, 3, 4};
    int dataB[] = {4, 3, 2, 1};
    SquareMatrix<int> a(dataA, 2);
    SquareMatrix<int> b(dataB, 2);

    SquareMatrix<int> sum = a + b;
    EXPECT_EQ(sum.Get(0, 0), 5);
    EXPECT_EQ(sum.Get(1, 1), 5);

    SquareMatrix<int> scaled = a * 3;
    EXPECT_EQ(scaled.Get(0, 1), 6);
    EXPECT_EQ(scaled.Get(1, 0), 9);
}

TEST(SquareMatrixTest, MultiplyAndNorm){
    int dataA[] = {1, 2, 3, 4};
    int dataB[] = {2, 0, 1, 2};
    SquareMatrix<int> a(dataA, 2);
    SquareMatrix<int> b(dataB, 2);

    SquareMatrix<int> product = a * b;
    EXPECT_EQ(product.Get(0, 0), 4);
    EXPECT_EQ(product.Get(0, 1), 4);
    EXPECT_EQ(product.Get(1, 0), 10);
    EXPECT_EQ(product.Get(1, 1), 8);

    EXPECT_DOUBLE_EQ(a.Norm(), std::sqrt(30.0));
}