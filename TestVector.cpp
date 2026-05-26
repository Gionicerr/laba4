#include <gtest/gtest.h>
#include <cmath>
#include "Vector.h"

TEST(VectorTest, AddAndScalarProduct){
    int firstData[] = {1, 2, 3};
    int secondData[] = {4, 5, 6};
    Vector<int> first(firstData, 3);
    Vector<int> second(secondData, 3);

    Vector<int> sum = first.Add(second);
    EXPECT_EQ(sum.Get(0), 5);
    EXPECT_EQ(sum.Get(1), 7);
    EXPECT_EQ(sum.Get(2), 9);

    EXPECT_EQ(first.ScalarProductOfVectors(second), 32);
}

TEST(VectorTest, MultiplyByScalarAndNorm){
    int data[] = {3, 4};
    Vector<int> vector(data, 2);

    Vector<int> scaled = vector.MultyplyByScalar(2);
    EXPECT_EQ(scaled.Get(0), 6);
    EXPECT_EQ(scaled.Get(1), 8);
    EXPECT_DOUBLE_EQ(vector.Norm(), 5.0);
}