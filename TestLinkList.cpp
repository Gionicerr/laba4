#include <gtest/gtest.h>
#include "LinkedList.h"
#include "Exceptions.h"

TEST(LinkedListTest, EmptyList){
    LinkedList<int> list;
    EXPECT_EQ(list.GetLength(), 0);
}

TEST(LinkedListTest, AppendPrepend){
    LinkedList<int> list;
    list.Append(20);
    list.Prepend(10);
    EXPECT_EQ(list.GetLength(), 2);
    EXPECT_EQ(list.GetFirst(), 10);
    EXPECT_EQ(list.GetLast(), 20);
}

TEST(LinkedListTest, InsertAt){
    LinkedList<int> list;
    list.Append(1);
    list.Append(3);
    list.InsertAt(1, 2);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.GetLength(), 3);
}

TEST(LinkedListTest, CacheLogic){
    LinkedList<int> list;
    for(int i = 0; i < 10; i++){
        list.Append(i);
    }
    for(int i = 0; i < 10; i++){
        EXPECT_EQ(list.Get(i), i);
    }
}

TEST(LinkedListTest, SubList){
    int array[] = {0, 1, 2, 3, 4};
    LinkedList<int> list(array, 5);
    LinkedList<int>* sub = list.GetSubList(1, 3);
    EXPECT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 1);
    EXPECT_EQ(sub->Get(2), 3);
    delete sub;
}