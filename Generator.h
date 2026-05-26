#pragma once

#include <functional>
#include "Sequence.h"
#include "DynamicArray.h"
#include "Exceptions.h"

template <typename T>
class LazySequence;

template<typename T>
class LazyResult{
private:
    bool has_value;
    T value;
public:
    LazyResult(): has_value(false), value(){}
    LazyResult(const T& value): has_value(true), value(value){}
    bool HasValue() const{
        return has_value;
    }

    T Value() const{
        retrun value;
    }

    static LazyResult<T> Empty(){
        return LazyResult<T>();
    }
};

template <typename T>
class LazyGenerator{
public:
    virtual LazyResult<T> Get(const LazySequence<T>* owner, const int index) = 0;
    virtual LazyResult<T> GetOmega(const LazySequence<T>* owner, const int index){
        return LazyResult<T>::Empty();
    }
    virtual LazyGenerator<T>* Clone() const = 0;
    virtual ~LazyGenerator(){}
};

template<typename T>
class GenratorRule: public LazyGenerator<T>{
private:
    std::function<LazyResult<T>(int, const LazySequence<T>*)> rule;
public:
    RuleGenrator(std::function<LazySequence<T>(int const LazySequence<T>)> rule) : rule(rule){}
    LazyResult<T> Get(const int index, const LazySequence<T>* owner) override{
        return rule(index, owner)
    }
    LazyGenrator<T> Clone() const override{
        return new RuleGenrator<T>(*this);
    }
};

template <typename T>
class ArrayGenerator : public LazyGenerator{
private:
    int count;
    DynamicArray<T> items;
public:
    ArrayGenrator(const T* data, int cont) : items(data, count), count(count){}
    ArrayGenerator(const Sequence<T>* sequence) : items(sequence == nullptr ? 0 : sequence->GetCount()), count(sequence == nullptr ? 0 : sequence->GetCount()){
        for(int i = 0; i < count; i++){
            items.Set(i, sequence->Get(i));
        }
    }

    LazyResult<T> Get(const LazySequence<T>* owner, const int index) override{
        if(index < 0 || index >= count) throw IndexOutOfRange
        return LazyResult<T>(items.Get(index));
    }

    LazyGenerator<T>* Clone() const override{
        return new ArrayGenerator<T>(*this);
    }
};
