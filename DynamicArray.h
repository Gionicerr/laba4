#pragma once

#include "Exceptions.h"

template <typename T>
class DynamicArray{
private:
    T* items;
    int size;
public:
    DynamicArray(const T* source_items, int count){
        if(count < 0) throw InvalidSize();
        if(source_items == nullptr && count > 0) throw NullPointer();
        size = count;
        items = new T[size];
        for(int i = 0; i < size; i++){
            items[i] = source_items[i];
        }
    }

    DynamicArray(int count){
        if(count < 0) throw InvalidSize();
        size = count;
        items = new T[size]();
    }
    
    DynamicArray(const DynamicArray<T>& other){
        size = other.size;
        items = new T[size];
        for(int i = 0; i < size; i++){
            items[i] = other.items[i];
        }
    }

    ~DynamicArray(){
        delete[] items;
    }

    DynamicArray<T>& operator=(const DynamicArray<T>& other){
        if(this != &other){
            delete[] items;
            size = other.size;
            items = new T[size];
            for(int i = 0; i < size; i++){
                items[i] = other.items[i];
            }
        }
        return *this;
    }

    T& operator[](int index){
        if(index < 0 || index >= size) throw IndexOutOfRange();
        return items[index];
    }

    const T& operator[](int index) const{
        if(index < 0 || index >= size) throw IndexOutOfRange();
        return items[index];
    }

    T Get(int index) const{
        if(index < 0 || index >= size) throw IndexOutOfRange();
        return items[index];
    }

    int GetSize() const{
        return size;
    }
    
    void Set(int index, T value){
        if(index < 0 || index >= size) throw IndexOutOfRange();
        items[index] = value;
    }
    
    void Resize(int new_size){
        if(new_size < 0) throw InvalidSize();
        T* new_items = new T[new_size]();
        int copy_count = (new_size < size) ? new_size : size;
        for(int i = 0; i < copy_count; i++){
            new_items[i] = items[i];
        }
        delete[] items;
        items = new_items;
        size = new_size;
    }
};
