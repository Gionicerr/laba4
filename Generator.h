#pragma once

#include <functional>
#include "Sequence.h"
#include "DynamicArray.h"
#include "Exceptions.h"
#include "Ordinal.h"

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
        return value;
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
    virtual LazyResult<T> GetOrdinal(const LazySequence<T>* owner, const Ordinal& index){
        return LazyResult<T>::Empty();
    }
    virtual LazyGenerator<T>* Clone() const = 0;
    virtual ~LazyGenerator(){}
};

template<typename T>
class RuleGenerator: public LazyGenerator<T>{
private:
    std::function<LazyResult<T>(int, const LazySequence<T>*)> rule;
public:
    RuleGenerator(std::function<LazyResult<T>(int, const LazySequence<T>*)> rule) : rule(rule){}
    LazyResult<T> Get(const LazySequence<T>* owner, const int index) override{
        return rule(index, owner);
    }
    LazyGenerator<T>* Clone() const override{
        return new RuleGenerator<T>(*this);
    }
};

template <typename T>
class ArrayGenerator : public LazyGenerator<T>{
private:
    int count;
    DynamicArray<T> items;
public:
    ArrayGenerator(const T* data, int count) : count(count), items(data, count){}
    ArrayGenerator(const Sequence<T>* sequence) : count(sequence == nullptr ? 0 : sequence->GetLength()), items(count){
        for(int i = 0; i < count; i++){
            items.Set(i, sequence->Get(i));
        }
    }

    LazyResult<T> Get(const LazySequence<T>* owner, const int index) override{
        if(index < 0 || index >= count) return LazyResult<T>::Empty();
        return LazyResult<T>(items.Get(index));
    }

    LazyGenerator<T>* Clone() const override{
        return new ArrayGenerator<T>(*this);
    }
};
