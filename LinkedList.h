#pragma once

#include "Exceptions.h"

template <typename T>
class LinkedList{
private:
    struct Node{
        T data;
        Node* next;
        Node(const T& data, Node* next = nullptr) : data(data), next(next) {}
    };
    Node* head;
    Node* tail;
    int length;
    mutable Node* cached_node;
    mutable int cached_index;
public:
    LinkedList() : head(nullptr), tail(nullptr), length(0), cached_node(nullptr), cached_index(-1) {}

    LinkedList(const T* items, int count) : LinkedList(){
        if(count < 0) throw InvalidSize();
        for(int i = 0; i < count; i++){
            Append(items[i]);
        }
    }

    LinkedList(const LinkedList<T>& other) : LinkedList(){
        for(int i = 0; i<other.GetLength(); i++){
            Append(other.Get(i));
        }
    }

    ~LinkedList(){
        Node* current = head;
        while(current != nullptr){
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    int GetLength() const{
        return length;
    }

    T GetFirst() const{
        if(length == 0) throw IndexOutOfRange();
        return head->data;
    }

    T GetLast() const{
        if(length == 0) throw IndexOutOfRange();
        return tail->data;
    }

    T Get(int index) const{
        if(index < 0 || index >= length) throw IndexOutOfRange();
        Node* current = head;
        int current_index = 0;
        if(cached_node != nullptr && cached_index <= index){
            current = cached_node;
            current_index = cached_index;
        }
        while(current_index < index){
            current = current->next;
            current_index++;
        }
        cached_node = current;
        cached_index = current_index;
        return current->data;
    }

    void Set(int index, const T& item){
        if(index < 0 || index >= length) throw IndexOutOfRange();
        GetRef(index) = item;
    }
    
    void RemoveAt(int index){
        if(index < 0 || index >= length) throw IndexOutOfRange();
        Node** current = &head;
        for(int i = 0; i < index; i++){
            current = &((*current)->next);
        }
        Node* to_delete = *current;
        *current = to_delete->next;
        if(to_delete == tail){
            tail = (index == 0) ? nullptr : head;
            if(tail != nullptr){
                while(tail->next != nullptr) tail = tail->next;
            }
        }
        delete to_delete;
        length--;
        cached_node = nullptr;
        cached_index = -1;
        if(length == 0){
            head = nullptr;
            tail = nullptr;
        }
    }
    
    T& GetRef(int index){
        if(index < 0 || index >= length) throw IndexOutOfRange();
        Get(index);
        return cached_node->data;
    }

    const T& GetRef(int index) const{
        if(index < 0 || index >= length) throw IndexOutOfRange();
        Get(index);
        return cached_node->data;
    }

    void InsertAt(int index, const T& item){
        if(index < 0 || index > length) throw IndexOutOfRange();
        Node** current;
        int current_index = 0;
        if(cached_node != nullptr && index > cached_index){
            current = &(cached_node->next);
            current_index = cached_index + 1;
        }
        else{
            current = &head;
            current_index = 0;
        }
        for(int i = current_index; i < index; i++){
            current = &((*current)->next);
        }
        Node* new_node = new Node(item, *current);
        *current = new_node;
        if(index == length){
            tail = new_node;
        }
        if(cached_node != nullptr && index <= cached_index){
            cached_index++;
        }
        cached_node = new_node;
        cached_index = index;
        length++;
        
    }

    void Append(const T& item){
        InsertAt(length, item);
    }

    void Prepend(const T& item){
        InsertAt(0, item);
    }
    
    LinkedList<T>* GetSubList(int start_index, int end_index) const{
        if(start_index < 0 || end_index >= length || start_index > end_index) throw IndexOutOfRange();
        LinkedList<T>* new_list = new LinkedList<T>();
        for(int i = start_index; i <= end_index; i++){
            new_list->Append(this->Get(i));
        }
        return new_list;
    }

    LinkedList<T>* Concat(LinkedList<T>* other) const{
        LinkedList<T>* result = new LinkedList<T>(*this);
        for(int i = 0; i < other->GetLength(); i++){
            result->Append(other->Get(i));
        }
        return result;
    }
};
