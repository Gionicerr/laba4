#include <gtest/gtest.h>
#include "Deque.h"

TEST(DequeTest, PushAndPop){
    Deque<int> deque;
    deque.PushFront(2);
    deque.PushBack(3);
    deque.PushFront(1);

    EXPECT_EQ(deque.PopFront(), 1);
    EXPECT_EQ(deque.PopBack(), 3);
    EXPECT_EQ(deque.Get(0), 2);
}

TEST(DequeTest, ConcatAndSubDeque){
    int leftData[] = {1, 2};
    int rightData[] = {3, 4};
    Deque<int> left(leftData, 2);
    Deque<int> right(rightData, 2);

    Deque<int> joined = left.Concat(right);
    EXPECT_EQ(joined.Get(2), 3);
    EXPECT_EQ(joined.Get(3), 4);

    Deque<int> sub = joined.GetSubDeque(1, 2);
    EXPECT_EQ(sub.Get(0), 2);
    EXPECT_EQ(sub.Get(1), 3);
}