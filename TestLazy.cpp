#include <gtest/gtest.h>
#include "LazySequence.h"

// Лямбда-правило для первой бесконечной последовательности: 0, 10, 20, 30, 40...
auto inf_rule_1 = [](int index, const LazySequence<int>*) {
    return LazyResult<int>(index * 10);
};

// Лямбда-правило для второй бесконечной последовательности: 5, 15, 25, 35, 45...
auto inf_rule_2 = [](int index, const LazySequence<int>*) {
    return LazyResult<int>(index * 10 + 5);
};

// Тест базового функционала на конечной последовательности
TEST(LazySequenceTest, Basics) {
    int data[] = {1, 2, 3, 4, 5};
    LazySequence<int> fin(5, data);

    EXPECT_EQ(fin.GetLength(), 5);
    EXPECT_EQ(fin.GetLengthKind(), LazyKind::Finite);
    EXPECT_EQ(fin.Get(0), 1);
    EXPECT_EQ(fin.Get(4), 5);
    
    // Проверка корректности выброса исключений при выходе за границы
    EXPECT_THROW(fin.Get(5), IndexOutOfRange);
    EXPECT_THROW(fin.Get(-1), IndexOutOfRange);
}

// Тест конкатенации двух бесконечных последовательностей (Трансфинитное пространство w + w)
TEST(LazySequenceTest, ConcatTwoInfinite) {
    LazySequence<int> inf1(inf_rule_1, LazyKind::Infinite);
    LazySequence<int> inf2(inf_rule_2, LazyKind::Infinite);

    Sequence<int>* concat_base = inf1.Concat(&inf2);
    auto* lazy_concat = dynamic_cast<LazySequence<int>*>(concat_base);
    
    ASSERT_NE(lazy_concat, nullptr); // Если каст провалился, прерываем этот тест
    EXPECT_EQ(lazy_concat->GetLengthKind(), LazyKind::Infinite);

    // 1. Проверяем элементы первой (левой) бесконечной последовательности через Get()
    EXPECT_EQ(lazy_concat->Get(0), 0);   // inf1[0]
    EXPECT_EQ(lazy_concat->Get(1), 10);  // inf1[1]
    EXPECT_EQ(lazy_concat->Get(5), 50);  // inf1[5]

    // 2. Проверяем элементы второй (правой) бесконечной последовательности через омега-оффсет.
    EXPECT_EQ(lazy_concat->GetOmega(1), 5);   // Вторая последовательность, элемент [0]
    EXPECT_EQ(lazy_concat->GetOmega(2), 15);  // Вторая последовательность, элемент [1]
    EXPECT_EQ(lazy_concat->GetOmega(10), 95); // Вторая последовательность, элемент [9]

    // Граничные условия для GetOmega (оффсет должен быть >= 1)
    EXPECT_THROW(lazy_concat->GetOmega(0), IndexOutOfRange);

    delete concat_base;
}

// Тест операции Take (взятие первых N элементов)
TEST(LazySequenceTest, Take) {
    // 1. Take от бесконечной последовательности (превращает её в конечную)
    LazySequence<int> inf(inf_rule_1, LazyKind::Infinite);
    LazySequence<int>* taken_inf = inf.Take(4);

    EXPECT_EQ(taken_inf->GetLength(), 4);
    EXPECT_EQ(taken_inf->GetLengthKind(), LazyKind::Finite);
    EXPECT_EQ(taken_inf->Get(0), 0);
    EXPECT_EQ(taken_inf->Get(3), 30);
    EXPECT_THROW(taken_inf->Get(4), IndexOutOfRange);

    // 2. Take от конечной последовательности
    int data[] = {100, 200, 300};
    LazySequence<int> fin(3, data);
    LazySequence<int>* taken_fin = fin.Take(2);

    EXPECT_EQ(taken_fin->GetLength(), 2);
    EXPECT_EQ(taken_fin->Get(0), 100);
    EXPECT_EQ(taken_fin->Get(1), 200);
    EXPECT_THROW(taken_fin->Get(2), IndexOutOfRange);

    delete taken_inf;
    delete taken_fin;
}

// Тест взятия подпоследовательности (среза)
TEST(LazySequenceTest, Subsequence) {
    LazySequence<int> inf(inf_rule_1, LazyKind::Infinite); // 0, 10, 20, 30, 40, 50...
    Sequence<int>* sub_base = inf.GetSubsequence(2, 4);    // Индексы 2, 3, 4 -> 20, 30, 40

    EXPECT_EQ(sub_base->GetLength(), 3);
    EXPECT_EQ(sub_base->Get(0), 20);
    EXPECT_EQ(sub_base->Get(1), 30);
    EXPECT_EQ(sub_base->Get(2), 40);
    EXPECT_THROW(sub_base->Get(3), IndexOutOfRange);

    delete sub_base;
}

// Тест операций вставки (одиночной и целой последовательности)
TEST(LazySequenceTest, Inserts) {
    // Вставка бесконечной последовательности в бесконечную по индексу 2
    LazySequence<int> base_inf(inf_rule_1, LazyKind::Infinite);     // 0, 10, 20, 30, 40...
    LazySequence<int> insert_inf(inf_rule_2, LazyKind::Infinite);   // 5, 15, 25, 35, 45...

    LazySequence<int>* res_inf = base_inf.InsertSequenceAt(&insert_inf, 2);

    // До индекса 2 должны идти элементы из base_inf
    EXPECT_EQ(res_inf->Get(0), 0);   // base_inf[0]
    EXPECT_EQ(res_inf->Get(1), 10);  // base_inf[1]

    // Начиная с индекса 2 идет бесконечная вставленная последовательность insert_inf
    EXPECT_EQ(res_inf->Get(2), 5);   // insert_inf[0]
    EXPECT_EQ(res_inf->Get(3), 15);  // insert_inf[1]
    EXPECT_EQ(res_inf->Get(10), 85); // insert_inf[8]

    // Хвост исходной base_inf вытесняется во вторую омегу
    EXPECT_EQ(res_inf->GetOmega(1), 20); // base_inf[2]
    EXPECT_EQ(res_inf->GetOmega(2), 30); // base_inf[3]

    // Одиночная вставка InsertAt
    LazySequence<int> fin_base(inf_rule_1, LazyKind::Infinite);
    Sequence<int>* single_insert_base = fin_base.InsertAt(999, 1);
    EXPECT_EQ(single_insert_base->Get(0), 0);
    EXPECT_EQ(single_insert_base->Get(1), 999);
    EXPECT_EQ(single_insert_base->Get(2), 10);

    delete res_inf;
    delete single_insert_base;
}

// Точка входа для запуска всех тестов Google Test
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}