#include <gtest/gtest.h>

#include "LazySequence.h"
#include "MutableArraySequence.h"

LazyResult<int> EvenRule(int index, const LazySequence<int>*){
    return LazyResult<int>(index * 2);
}

LazyResult<int> TenRule(int index, const LazySequence<int>*){
    return LazyResult<int>(index * 10);
}

LazyResult<int> HundredRule(int index, const LazySequence<int>*){
    return LazyResult<int>(index * 100);
}

TEST(LazySequenceTest, TakeFromInfiniteAndFinite){
    LazySequence<int> infinite(EvenRule, Ordinal::Omega(1, 0));
    LazySequence<int>* first_five = infinite.Take(5);

    EXPECT_EQ(first_five->GetLength(), 5);
    EXPECT_EQ(first_five->Get(0), 0);
    EXPECT_EQ(first_five->Get(4), 8);
    EXPECT_THROW(first_five->Get(5), IndexOutOfRange);

    int data[] = {7, 8, 9, 10};
    LazySequence<int> finite(4, data);
    LazySequence<int>* first_two = finite.Take(2);

    EXPECT_EQ(first_two->GetLength(), 2);
    EXPECT_EQ(first_two->Get(0), 7);
    EXPECT_EQ(first_two->Get(1), 8);
    EXPECT_THROW(first_two->Get(2), IndexOutOfRange);

    delete first_five;
    delete first_two;
}

TEST(LazySequenceTest, SubsequenceFromInfinite){
    LazySequence<int> infinite(EvenRule, Ordinal::Omega(1, 0));
    Sequence<int>* sub = infinite.GetSubsequence(2, 5);

    EXPECT_EQ(sub->GetLength(), 4);
    EXPECT_EQ(sub->Get(0), 4);
    EXPECT_EQ(sub->Get(1), 6);
    EXPECT_EQ(sub->Get(3), 10);
    EXPECT_THROW(sub->Get(4), IndexOutOfRange);

    delete sub;
}

TEST(LazySequenceTest, AppendAndPrepend){
    LazySequence<int> infinite(EvenRule, Ordinal::Omega(1, 0));

    Sequence<int>* appended = infinite.Append(777);
    LazySequence<int>* appended_lazy = dynamic_cast<LazySequence<int>*>(appended);
    ASSERT_NE(appended_lazy, nullptr);
    EXPECT_EQ(appended_lazy->Get(3), 6);
    EXPECT_EQ(appended_lazy->Get(Ordinal::Omega(1, 0)), 777);

    Sequence<int>* prepended = infinite.Prepend(555);
    EXPECT_EQ(prepended->Get(0), 555);
    EXPECT_EQ(prepended->Get(1), 0);
    EXPECT_EQ(prepended->Get(4), 6);

    delete appended;
    delete prepended;
}

TEST(LazySequenceTest, ConcatInfiniteWithInfinite){
    LazySequence<int> left(EvenRule, Ordinal::Omega(1, 0));
    LazySequence<int> right(TenRule, Ordinal::Omega(1, 0));
    LazySequence<int>* result = left.Concat(&right);

    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(4), 8);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 0)), 0);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 3)), 30);
    EXPECT_EQ(result->GetOrdinalLength().GetOmegaCount(), 2);

    delete result;
}

TEST(LazySequenceTest, ConcatInfiniteWithFinite){
    LazySequence<int> infinite(EvenRule, Ordinal::Omega(1, 0));
    int data[] = {100, 200, 300};
    LazySequence<int> finite(3, data);
    LazySequence<int>* result = infinite.Concat(&finite);

    EXPECT_EQ(result->Get(5), 10);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 0)), 100);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 2)), 300);
    EXPECT_THROW(result->Get(Ordinal::Omega(1, 3)), IndexOutOfRange);

    delete result;
}

TEST(LazySequenceTest, ConcatFiniteWithInfinite){
    int data[] = {4, 5, 6};
    LazySequence<int> finite(3, data);
    LazySequence<int> infinite(TenRule, Ordinal::Omega(1, 0));
    LazySequence<int>* result = finite.Concat(&infinite);

    EXPECT_EQ(result->Get(0), 4);
    EXPECT_EQ(result->Get(2), 6);
    EXPECT_EQ(result->Get(3), 0);
    EXPECT_EQ(result->Get(6), 30);

    delete result;
}

TEST(LazySequenceTest, ConcatThreeInfiniteSequences){
    LazySequence<int> first(EvenRule, Ordinal::Omega(1, 0));
    LazySequence<int> second(TenRule, Ordinal::Omega(1, 0));
    LazySequence<int> third(HundredRule, Ordinal::Omega(1, 0));

    LazySequence<int>* first_two = first.Concat(&second);
    LazySequence<int>* result = first_two->Concat(&third);

    EXPECT_EQ(result->Get(3), 6);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 4)), 40);
    EXPECT_EQ(result->Get(Ordinal::Omega(2, 5)), 500);
    EXPECT_EQ(result->GetOrdinalLength().GetOmegaCount(), 3);

    delete first_two;
    delete result;
}

TEST(LazySequenceTest, ConcatLazyWithPlainSequence){
    LazySequence<int> infinite(EvenRule, Ordinal::Omega(1, 0));
    int data[] = {11, 22};
    MutableArraySequence<int> plain(data, 2);
    Sequence<int>* plain_base = &plain;

    Sequence<int>* result_base = infinite.Concat(plain_base);
    LazySequence<int>* result = dynamic_cast<LazySequence<int>*>(result_base);
    ASSERT_NE(result, nullptr);

    EXPECT_EQ(result->Get(4), 8);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 0)), 11);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 1)), 22);

    delete result_base;
}

TEST(LazySequenceTest, InsertInfiniteIntoInfinite){
    LazySequence<int> base(TenRule, Ordinal::Omega(1, 0));
    LazySequence<int> inserted(EvenRule, Ordinal::Omega(1, 0));
    LazySequence<int>* result = base.InsertSequenceAt(&inserted, 2);

    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 10);
    EXPECT_EQ(result->Get(2), 0);
    EXPECT_EQ(result->Get(5), 6);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 0)), 20);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 2)), 40);
    EXPECT_EQ(result->GetOrdinalLength().GetOmegaCount(), 2);

    delete result;
}

TEST(LazySequenceTest, InsertInfiniteIntoFinite){
    int data[] = {1, 2, 3, 4};
    LazySequence<int> base(4, data);
    LazySequence<int> inserted(TenRule, Ordinal::Omega(1, 0));
    LazySequence<int>* result = base.InsertSequenceAt(&inserted, 2);

    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 0);
    EXPECT_EQ(result->Get(7), 50);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 0)), 3);
    EXPECT_EQ(result->Get(Ordinal::Omega(1, 1)), 4);
    EXPECT_EQ(result->GetOrdinalLength().GetOmegaCount(), 1);
    EXPECT_EQ(result->GetOrdinalLength().GetIndex(), 2);

    delete result;
}

TEST(LazySequenceTest, InsertFiniteIntoInfinite){
    LazySequence<int> base(TenRule, Ordinal::Omega(1, 0));
    int data[] = {7, 8, 9};
    LazySequence<int> inserted(3, data);
    LazySequence<int>* result = base.InsertSequenceAt(&inserted, 2);

    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 10);
    EXPECT_EQ(result->Get(2), 7);
    EXPECT_EQ(result->Get(3), 8);
    EXPECT_EQ(result->Get(4), 9);
    EXPECT_EQ(result->Get(5), 20);
    EXPECT_EQ(result->GetOrdinalLength().GetOmegaCount(), 1);
    EXPECT_EQ(result->GetOrdinalLength().GetIndex(), 0);

    delete result;
}

TEST(LazySequenceTest, InsertFiniteIntoFinite){
    int base_data[] = {1, 4};
    int insert_data[] = {2, 3};
    LazySequence<int> base(2, base_data);
    LazySequence<int> inserted(2, insert_data);
    LazySequence<int>* result = base.InsertSequenceAt(&inserted, 1);

    ASSERT_EQ(result->GetLength(), 4);
    EXPECT_EQ(result->Get(0), 1);
    EXPECT_EQ(result->Get(1), 2);
    EXPECT_EQ(result->Get(2), 3);
    EXPECT_EQ(result->Get(3), 4);

    delete result;
}

TEST(LazySequenceTest, InsertPlainSequenceIntoInfinite){
    LazySequence<int> base(TenRule, Ordinal::Omega(1, 0));
    int data[] = {42, 43};
    MutableArraySequence<int> plain(data, 2);
    Sequence<int>* plain_base = &plain;

    LazySequence<int>* result = base.InsertSequenceAt(plain_base, 1);

    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 42);
    EXPECT_EQ(result->Get(2), 43);
    EXPECT_EQ(result->Get(3), 10);

    delete result;
}

TEST(LazySequenceTest, InsertAtSingleValue){
    LazySequence<int> base(TenRule, Ordinal::Omega(1, 0));
    Sequence<int>* result = base.InsertAt(999, 1);

    EXPECT_EQ(result->Get(0), 0);
    EXPECT_EQ(result->Get(1), 999);
    EXPECT_EQ(result->Get(2), 10);

    delete result;
}

int main(int argc, char** argv){
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}