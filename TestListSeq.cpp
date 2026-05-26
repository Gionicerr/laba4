#include <gtest/gtest.h>
#include "ListSequence.h"

TEST(ListSequenceTest, Operations) {
    ListSequence<int> list_seq;
    list_seq.Append(10)->Prepend(5);
    EXPECT_EQ(list_seq.GetLength(), 2);
    EXPECT_EQ(list_seq.Get(0), 5);
    EXPECT_EQ(list_seq.Get(1), 10);
}

TEST(ListSequenceTest, Concat) {
    int list_seq_1[] = {1, 2};
    int list_seq_2[] = {3, 4};
    ListSequence<int> seq_1(list_seq_1, 2);
    ListSequence<int> seq_2(list_seq_2, 2);
    seq_1.Concat(&seq_2);
    EXPECT_EQ(seq_1.GetLength(), 4);
    EXPECT_EQ(seq_1.Get(2), 3);
}