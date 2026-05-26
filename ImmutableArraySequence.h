#pragma once

#include "ArraySequence.h"

template <typename T>
class ImmutableArraySequence : public ArraySequence<T>{
protected:
    ArraySequence<T>* Instance() override{
        return new ImmutableArraySequence<T>(*this);
    }
public:
    ImmutableArraySequence() : ArraySequence<T>(){}
    ImmutableArraySequence(T* data, int count) : ArraySequence<T>(data,count){}
    ImmutableArraySequence(const ArraySequence<T>& other) : ArraySequence<T>(other){}
};