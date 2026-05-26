#include <gtest/gtest.h>
#include "DynamicArray.h"
#include "Exceptions.h"

TEST(DynamicArrayTest, DefaultConstructor){
    DynamicArray<int> array(5);
    EXPECT_EQ(array.GetSize(), 5);
}

TEST(DynamicArrayTest, ConstructorFromBuffer){
    int items[] = {1, 2, 3};
    DynamicArray<int> array(items, 3);
    EXPECT_EQ(array.GetSize(), 3);
    EXPECT_EQ(array.Get(0), 1);
    EXPECT_EQ(array.Get(2), 3);
}

TEST(DynamicArrayTest, SetAndGet){
    DynamicArray<int> array(2);
    array.Set(0, 100);
    array.Set(1, 200);
    EXPECT_EQ(array.Get(0), 100);
    EXPECT_EQ(array.Get(1), 200);
}

TEST(DynamicArrayTest, Resize){
    DynamicArray<int> array(2);
    array.Set(0, 1);
    array.Resize(5);
    EXPECT_EQ(array.GetSize(), 5);
    EXPECT_EQ(array.Get(0), 1);
    
    array.Resize(1);
    EXPECT_EQ(array.GetSize(), 1);
}

TEST(DynamicArrayTest, Exceptions){
    DynamicArray<int> array(2);
    EXPECT_THROW(array.Get(2), IndexOutOfRange);
    EXPECT_THROW(array.Get(-1), IndexOutOfRange);
    EXPECT_THROW(DynamicArray<int>(-1), InvalidSize);
}