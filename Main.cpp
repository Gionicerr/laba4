#include <iostream>
#include <limits>
#include <string>

#include "DynamicArray.h"
#include "LazySequence.h"

class LazyStore{
private:
    DynamicArray<LazySequence<int>*> items;
    int count;

public:
    LazyStore() : items(4), count(0){}

    ~LazyStore(){
        for(int i = 0; i < count; i++){
            delete items.Get(i);
        }
    }

    int GetCount() const{
        return count;
    }

    LazySequence<int>* Get(int index) const{
        if(index < 0 || index >= count) throw IndexOutOfRange();
        return items.Get(index);
    }

    int Add(LazySequence<int>* sequence){
        if(sequence == nullptr) throw NullPointer();
        if(count == items.GetSize()){
            items.Resize(items.GetSize() * 2);
        }
        items.Set(count, sequence);
        count++;
        return count - 1;
    }
};

LazyResult<int> LinearRule(int index, const LazySequence<int>*, int multiplier, int offset){
    return LazyResult<int>(index * multiplier + offset);
}

class LinearGenerator: public LazyGenerator<int>{
private:
    int multiplier;
    int offset;

public:
    LinearGenerator(int multiplier, int offset) : multiplier(multiplier), offset(offset){}

    LazyResult<int> Get(const LazySequence<int>* owner, const int index) override{
        return LinearRule(index, owner, multiplier, offset);
    }

    LazyGenerator<int>* Clone() const override{
        return new LinearGenerator(*this);
    }
};

int ReadInt(const std::string& prompt){
    int value;
    while(true){
        std::cout<<prompt;
        if(std::cin >> value){
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout<<"Enter int.\n";
    }
}

void PrintSequences(const LazyStore& store){
    if(store.GetCount() == 0){
        std::cout<<"No sequences.\n";
        return;
    }
    for(int i = 0; i < store.GetCount(); i++){
        Ordinal length = store.Get(i)->GetOrdinalLength();
        std::cout<<i<<": length = ";
        if(length.IsFinite()){
            std::cout<<length.ToInt();
        }
        else{
            std::cout<<"omega *"<<length.GetOmegaCount();
            if(length.GetIndex() != 0){
                std::cout<<" + "<<length.GetIndex();
            }
        }
        std::cout<<'\n';
    }
}

LazySequence<int>* ReadSequence(LazyStore& store, const std::string& prompt){
    if(store.GetCount() == 0){
        std::cout<<"Create at least one sequence.\n";
        return nullptr;
    }
    int index = ReadInt(prompt);
    if(index < 0 || index >= store.GetCount()){
        std::cout<<"No such sequence.\n";
        return nullptr;
    }
    return store.Get(index);
}

void StoreLazyPointer(LazyStore& store, LazySequence<int>* sequence){
    int index = store.Add(sequence);
    std::cout<<"Created sequence #"<<index<<".\n";
}

void StoreSequencePointer(LazyStore& store, Sequence<int>* sequence){
    LazySequence<int>* lazy = dynamic_cast<LazySequence<int>*>(sequence);
    if(lazy == nullptr){
        LazySequence<int>* wrapped = new LazySequence<int>(sequence);
        delete sequence;
        StoreLazyPointer(store, wrapped);
        return;
    }
    StoreLazyPointer(store, lazy);
}

void CreateFinite(LazyStore& store){
    int count = ReadInt("Element count: ");
    if(count < 0){
        std::cout<<"Negative size!.\n";
        return;
    }
    DynamicArray<int> values(count);
    for(int i = 0; i < count; i++){
        values.Set(i, ReadInt("element[" + std::to_string(i) + "] = "));
    }

    int* raw_values = new int[count];
    for(int i = 0; i < count; i++){
        raw_values[i] = values.Get(i);
    }
    StoreLazyPointer(store, new LazySequence<int>(count, raw_values));
    delete[] raw_values;
}

void CreateInfinite(LazyStore& store){
    int multiplier = ReadInt("index * [] + (). [] = ");
    int offset = ReadInt("() = ");
    StoreLazyPointer(store, new LazySequence<int>(new LinearGenerator(multiplier, offset), Ordinal::Omega(1, 0)));
}

void GetElement(LazyStore& store){
    LazySequence<int>* sequence = ReadSequence(store, "Sequence id: ");
    if(sequence == nullptr) return;

    int kind = ReadInt("0 = finite index\n1 = ordinal + index: ");
    try{
        if(kind == 0){
            int index = ReadInt("index = ");
            std::cout<<sequence->Get(index)<<'\n';
        }
        else{
            int omega_count = ReadInt("omega block = ");
            int index = ReadInt("index inside block = ");
            std::cout<<sequence->Get(Ordinal::Omega(omega_count, index))<<'\n';
        }
    }
    catch(const std::exception& error){
        std::cout<<"Error: "<<error.what()<<'\n';
    }
}

void ConcatSequences(LazyStore& store){
    LazySequence<int>* left = ReadSequence(store, "Left sequence id: ");
    if(left == nullptr) return;
    LazySequence<int>* right = ReadSequence(store, "Right sequence id: ");
    if(right == nullptr) return;
    StoreLazyPointer(store, left->Concat(right));
}

void TakeSequence(LazyStore& store){
    LazySequence<int>* sequence = ReadSequence(store, "Sequence id: ");
    if(sequence == nullptr) return;
    int count = ReadInt("Take count: ");
    try{
        StoreLazyPointer(store, sequence->Take(count));
    }
    catch(const std::exception& error){
        std::cout<<"Error: "<<error.what()<<'\n';
    }
}

void Subsequence(LazyStore& store){
    LazySequence<int>* sequence = ReadSequence(store, "Sequence id: ");
    if(sequence == nullptr) return;
    int start = ReadInt("start = ");
    int end = ReadInt("end = ");
    try{
        StoreSequencePointer(store, sequence->GetSubsequence(start, end));
    }
    catch(const std::exception& error){
        std::cout<<"Error: "<<error.what()<<'\n';
    }
}

void AppendOrPrepend(LazyStore& store, bool append){
    LazySequence<int>* sequence = ReadSequence(store, "Sequence id: ");
    if(sequence == nullptr) return;
    int value = ReadInt("value = ");
    if(append){
        StoreSequencePointer(store, sequence->Append(value));
    }
    else {
        StoreSequencePointer(store, sequence->Prepend(value));
    }
}

void InsertSequence(LazyStore& store){
    LazySequence<int>* base = ReadSequence(store, "Base sequence id: ");
    if(base == nullptr) return;
    LazySequence<int>* inserted = ReadSequence(store, "Inserted sequence id: ");
    if(inserted == nullptr) return;
    int index = ReadInt("Insert position: ");
    try{
        StoreLazyPointer(store, base->InsertSequenceAt(inserted, index));
    }
    catch(const std::exception& error){
        std::cout<<"Error: "<<error.what()<<'\n';
    }
}

void InsertValue(LazyStore& store){
    LazySequence<int>* base = ReadSequence(store, "Base sequence id: ");
    if(base == nullptr) return;
    int value = ReadInt("value = ");
    int index = ReadInt("Insert position: ");
    try{
        StoreSequencePointer(store, base->InsertAt(value, index));
    }
    catch(const std::exception& error){
        std::cout<<"Error: "<<error.what()<<'\n';
    }
}

void PrintMenu(){
    std::cout<<"\n=== LazySequence UI ===\n"
             <<"1. Create finite sequence\n"
             <<"2. Create infinite sequence\n"
             <<"3. List sequences\n"
             <<"4. Get element\n"
             <<"5. Concat\n"
             <<"6. Take\n"
             <<"7. Subsequence\n"
             <<"8. Append\n"
             <<"9. Prepend\n"
             <<"10. InsertSequenceAt\n"
             <<"11. InsertAt value\n"
             <<"0. Exit\n";
}

int main(){
    LazyStore store;
    while(true){
        PrintMenu();
        int command = ReadInt("command = ");
        switch(command){
            case 1: CreateFinite(store); break;
            case 2: CreateInfinite(store); break;
            case 3: PrintSequences(store); break;
            case 4: GetElement(store); break;
            case 5: ConcatSequences(store); break;
            case 6: TakeSequence(store); break;
            case 7: Subsequence(store); break;
            case 8: AppendOrPrepend(store, true); break;
            case 9: AppendOrPrepend(store, false); break;
            case 10: InsertSequence(store); break;
            case 11: InsertValue(store); break;
            case 0: return 0;
            default: std::cout<<"Unknown command.\n"; break;
        }
    }
}