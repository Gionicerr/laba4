#pragma once

#include "LinkedList.h"
#include "Sequence.h"

template <typename T>
class ListSequence : public Sequence<T>{
private:
    LinkedList<T>* items;
public:
    ListSequence(){
        items = new LinkedList<T>();
    }

    ListSequence(T* data, int count){
        items = new LinkedList<T>(data, count);
    }

    ListSequence(const LinkedList<T>& list){
        items = new LinkedList<T>(list);
    }

    ListSequence(const ListSequence<T>& other){
        items = new LinkedList<T>(*(other.items));
    }

    ~ListSequence(){
        delete items;
    }

    T Get(const int index) const override{
        return items->Get(index);
    }

    T GetFirst() const override{
        return items->GetFirst();
    }

    T GetLast() const override{
        return items->GetLast();
    }

    void Set(int index, const T& value){
        items->Set(index, value);
    }

    void RemoveAt(int index){
        items->RemoveAt(index);
    }

    T& GetRef(int index){
        return items->GetRef(index);
    }

    const T& GetRef(int index) const{
        return items->GetRef(index);
    }
    
    int GetLength() const override{
        return items->GetLength();
    }

    Sequence<T>* Append(const T& value) override{
        items->Append(value);
        return this;
    }

    Sequence<T>* Prepend(const T& value) override{
        items->Prepend(value);
        return this;
    }

    Sequence<T>* InsertAt(const T& value, int index) override{
        items->InsertAt(index, value);
        return this;
    }

    Sequence<T>* Concat(const Sequence<T>* other) override{
        for(int i = 0; i < other->GetLength(); i++){
            items->Append(other->Get(i));
        }
        return this;
    }
    
    Sequence<T>* GetSubsequence(const int start_index, const int end_index) override{
        ListSequence<T>* result = new ListSequence<T>();
        for(int i = start_index; i <= end_index; i++){
            result->Append(items->Get(i));
        }
        return result;
    }
};
