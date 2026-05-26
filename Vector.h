#pragma once
#include "MutableArraySequence.h"
#include "Exceptions.h"

template <typename T>
class Vector{
private:
    MutableArraySequence<T> items;

    void CheckSameDimension(const Vector<T>& other)const{
        if(GetDimension() != other.GetDimension()){
            throw InvalidSize();
        }
    }
public:
    Vector() : items(0){}

    Vector(int dimension) : items(dimension){
        if(dimension < 0) throw InvalidSize();
    }

    Vector(T* data, int size) : items(data,size){
        if(size < 0) throw InvalidSize();
    }
    
    int GetDimension() const{
        return items.GetLength();
    }
    
    T& operator()(int index){
        return items(index);
    }

    const T& operator()(int index) const{
        return items(index);
    }

    T Get(int index) const{
        return items.Get(index);
    }

    Vector<T> Add(const Vector<T>& other) const{
        CheckSameDimension(other);
        Vector<T> result(GetDimension());
        for(int i = 0; i < GetDimension(); i++){
            result(i) = Get(i) + other.Get(i);
        }
        return result;
    }

    Vector<T> MultyplyByScalar(const T& scalar) const{
        Vector<T> result(GetDimension());
        for(int i = 0; i < GetDimension(); i++){
            result(i) = Get(i) * scalar;
        }
        return result;
    }

    T ScalarProductOfVectors(const Vector<T>& other) const{
        CheckSameDimension(other);
        T result = T();
        for(int i = 0; i < GetDimension(); i++){
            result = result + Get(i) * other.Get(i);
        }
        return result;
    }

    double Norm() const{
        double result = 0.0;
        for(int i = 0; i <GetDimension(); i++){
            double value = Get(i);
            result += value * value;
        }
        return std::sqrt(result);
    }
};