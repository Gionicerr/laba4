#pragma once
#include "ListSequence.h"
#include "Exceptions.h"

template <typename T>
class Deque{
private:
    int size;
    ListSequence<T> items;

    void CheckIndex(int index) const{
        if(index < 0 || index >= size) throw IndexOutOfRange();
    }

    static bool DefaultSort(T left, T right){
        return left < right;
    }
public:
    Deque() : size(0), items(ListSequence<T>()){}
    
    Deque(int count) : size(0), items(ListSequence<T>()){
        if(count < 0) throw InvalidSize();
        for(int i = 0; i < count; i++){
            items.Append(T());
        }
        size = count;
    }

    Deque(T* data, int count) : size(count), items(data, count){
        if(count < 0) throw InvalidSize();
        if(data == nullptr && count > 0) throw NullPointer();
    }

    T& operator()(int index){
        CheckIndex(index);
        return items.GetRef(index);
    }
    
    const T& operator()(int index) const{
        CheckIndex(index);
        return items.GetRef(index);
    }

    int GetSize() const{
        return size;
    }

    T Get(int index) const{
        CheckIndex(index);
        return items.Get(index);
    }
    
    void Set(int index, const T& value){
        CheckIndex(index);
        items.Set(index, value);
    }

    void PushBack(const T& value){
        items.Append(value);
        size++;
    }

    void PushFront(const T& value){
        items.Prepend(value);
        size++;
    }

    T PopBack(){
        if(size == 0) throw IndexOutOfRange();
        T result = items.Get(size - 1);
        items.RemoveAt(size - 1);
        size--;
        return result;
    }

    T PopFront(){
        if(size == 0) throw IndexOutOfRange();
        T result = items.Get(0);
        items.RemoveAt(0);
        size--;
        return result;
    }

    Deque<T> Concat(const Deque<T>& other) const{
        Deque<T> result;
        for(int i = 0; i < size; i++){
            result.PushBack(Get(i));
        }
        for(int i = 0; i < other.size; i++){
            result.PushBack(other.Get(i));
        }
        return result;
    }

    Deque<T> GetSubDeque(const int start_index, const int end_index) const{
        if(start_index < 0 || end_index >= size || start_index > end_index) throw IndexOutOfRange();
        Deque<T> result;
        for(int i = start_index; i <= end_index; i++){
            result.PushBack(Get(i));
        }
        return result;
    }

    bool ContainSubDeque(const Deque<T>& sub) const{
        if(sub.size == 0) return true;
        if(sub.size > size) return false;
        for(int i = 0; i <= size - sub.size; i++){
            bool found = true;
            for(int j = 0; j < sub.size; j++){
                if(Get(i + j) != sub.Get(j)){
                    found = false;
                    break;
                }
            }
            if(found) return true;
        }
        return false;
    }

    Deque<T> Map(T (*oper)(T)) const{
        if(oper == nullptr) throw NullPointer();
        Deque<T> result;
        for(int i = 0; i < size; i++){
            result.PushBack(oper(Get(i)));
        }
        return result;
    }

    Deque<T> Where(bool (*condition)(T)) const{
        if(condition == nullptr) throw NullPointer();
        Deque<T> result;
        for(int i = 0; i < size; i++){
            if(condition(Get(i))) result.PushBack(Get(i));
        }
        return result;
    }

    T Reduce(T (*oper)(T,T), T start_value) const{
        if(oper == nullptr) throw NullPointer();
        T result = start_value;
        for(int i = 0; i < size; i++){
            result = oper(result, Get(i));
        }
        return result;
    }

    void Sort(bool (*condition)(T,T)){
        if(condition == nullptr) throw NullPointer();
        for(int i = 0; i < size-1; i++){
            int min_index = i;
            for(int j = i + 1; j < size; j++){
                if(condition((*this)(j), (*this)(min_index))){
                    min_index = j;
                }
            }
            if(min_index != i){
                T temp = (*this)(i);
                (*this)(i) = (*this)(min_index);
                (*this)(min_index) = temp;
            }
        }
    }

    void Sort(){
        Sort(DefaultSort);
    }

    Deque<T> Merge(const Deque<T>& other, bool (*condition)(T,T)) const{
        if(condition == nullptr) throw NullPointer();
        Deque<T> result = Concat(other);
        result.Sort(condition);
        return result;
    }
    
    Deque<T> Merge(const Deque<T>& other) const{
        return Merge(other, DefaultSort);
    }
};