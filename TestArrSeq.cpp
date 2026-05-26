#include <gtest/gtest.h>
#include "MutableArraySequence.h"
#include "ImmutableArraySequence.h"

TEST(MutableArraySequenceTest, Operations){
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);
    seq.Append(4);
    EXPECT_EQ(seq.GetLength(), 4);
    EXPECT_EQ(seq.Get(3), 4);
}

TEST(ImmutableArraySequenceTest, Operations){
    int data[] = {1, 2};
    ImmutableArraySequence<int> seq(data, 2);
    
    Sequence<int>* new_seq = seq.Append(3);
    
    EXPECT_EQ(seq.GetLength(), 2);
    EXPECT_EQ(new_seq->GetLength(), 3);
    EXPECT_EQ(new_seq->Get(2), 3);
    
    delete new_seq;
}