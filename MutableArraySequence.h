#pragma once

#include "ArraySequence.h"

template <typename T>
class MutableArraySequence : public ArraySequence<T>{
protected:
    ArraySequence<T>* Instance() override{
        return this;
    }
public:
    MutableArraySequence() : ArraySequence<T>(){}
    MutableArraySequence(int count) : ArraySequence<T>(count){}
    MutableArraySequence(T* data, int count) : ArraySequence<T>(data,count){}
    MutableArraySequence(const ArraySequence<T>& other) : ArraySequence<T>(other){}
};