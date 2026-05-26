#pragma once

#include "DynamicArray.h"
#include "LinkedList.h"
#include "Sequence.h"

template <typename T>
class ArraySequence : public Sequence<T>{
protected:
    DynamicArray<T> items;
    int count;
    virtual ArraySequence<T>* Instance() = 0;
public:
    ArraySequence() : items(0), count(0){}

    ArraySequence(int size) : items(size), count(size){}

    ArraySequence(T* data, int count) : items(data, count), count(count){}

    ArraySequence(const DynamicArray<T>& data) : items(data), count(data.GetSize()){}

    ArraySequence(const LinkedList<T>& list) : items(list.GetLength()), count(list.GetLength()){
        for(int i = 0; i < list.GetLength(); i++){
            items.Set(i, list.Get(i));
        }
    }

    ArraySequence(const ArraySequence<T>& other) : items(other.items), count(other.count){}


    virtual ~ArraySequence(){}

    T& operator()(int index){
        return items[index];
    }

    const T& operator()(int index) const{
        return items[index];
    }

    T Get(const int index) const override{
        return items.Get(index);
    }

    T GetFirst() const override{
        return items.Get(0);
    }

    T GetLast() const override{
        return items.Get(items.GetSize() - 1);
    }

    int GetLength() const override{
        return items.GetSize();
    }

    int GetCount() const{
        return count;
    }

    Sequence<T>* Append(const T& item) override{
        ArraySequence<T>* inst = Instance();
        if(inst->count == inst->items.GetSize()){
            int new_capacity = (inst->count == 0) ? 1 : inst->count*2;
            inst->items.Resize(new_capacity);
        }
        inst->items.Set(inst->count, item);
        inst->count++;
        return inst;
    }

    Sequence<T>* Prepend(const T& item) override{
        ArraySequence<T>* inst = Instance();
        int old_size = inst->items.GetSize();
        inst->items.Resize(old_size + 1);
        for(int i = old_size; i > 0; i--){
            inst->items.Set(i, inst->items.Get(i - 1));
        }
        inst->items.Set(0, item);
        return inst;
    }

    Sequence<T>* InsertAt(const T& item, int index) override{
        ArraySequence<T>* inst = Instance();
        int old_size = inst->items.GetSize();
        if(index < 0 || index > old_size) throw IndexOutOfRange();
        inst->items.Resize(old_size + 1);
        for(int i = old_size; i > index; i--){
            inst->items.Set(i, inst->items.Get(i - 1));
        }
        inst->items.Set(index, item);
        return inst;
    }

    Sequence<T>* Concat(const Sequence<T>* other) override{
        ArraySequence<T>* inst = Instance();
        int this_len = inst->GetLength();
        int other_len = other->GetLength();
        inst->items.Resize(this_len + other_len);
        for(int i = 0; i < other_len; i++){
            inst->items.Set(this_len + i, other->Get(i));
        }
        return inst;
    }
    
    Sequence<T>* GetSubsequence(const int start_index, const int end_index) override{
        int size = GetLength();
        if(start_index < 0 || end_index >= size || start_index > end_index) throw IndexOutOfRange();
        int length = end_index - start_index + 1;
        DynamicArray<T> new_items(length);
        for(int i = 0; i < length; i++){
            new_items.Set(i, Get(start_index + i));
        }
        ArraySequence<T>* inst = Instance();
        inst->items = new_items;
        return inst;
    }
};


//######  ######  #    #  ######  #    #  #    #   #####
//#    #  #    #  #    #  #       ##  ##  #    #  #     #
//#    #  #    #  #    #  #       # ## #  #    #        #
//#    #  #    #  #    #  #       #    #  #    #       #
//#    #  #    #  ######  ######  #    #  ######      #
//#    #  #    #       #  #       #    #       #     #
//#    #  #    #       #  #       #    #       #     
//#    #  ######       #  ######  #    #  ######     #