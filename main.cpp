#include <iostream>
#include <string>
#include <limits>
#include <exception>
#include <cmath>
#include <iomanip>

#include "DynamicArray.h"
#include "Deque.h"
#include "Vector.h"
#include "SquareMatrix.h"

int SquareFunc(int x){
    return x * x;
}
bool IsEven(int x){
    return x % 2 == 0;
}
int SumReducer(int a, int b){
    return a + b;
}
bool DescendingSort(int a, int b){
    return a > b;
}

template <typename T>
T SafeInput(const std::string& prompt){
    T value;
    while(true){
        std::cout<<prompt;
        if(std::cin>>value){
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout<<"Input error. Try again.\n";
    }
}

void PrintDeque(const Deque<int>& dq, const std::string& name = "Deque"){
    std::cout<<name<<": [ ";
    for(int i = 0; ; i++){
        try{
            std::cout<<dq.Get(i)<<" ";
        }
        catch(...){
            break;
        }
    }
    std::cout<<"]\n";
}

void PrintVector(const Vector<int>& v, const std::string& name = "Vector"){
    std::cout<<name<<" (dim "<<v.GetDimension()<<"): ( ";
    for(int i = 0; i < v.GetDimension(); i++){
        std::cout<<v.Get(i)<<(i + 1 < v.GetDimension() ? ", " : " ");
    }
    std::cout<<")\n";
}

void Printmatatrix(const SquareMatrix<int>& mat, const std::string& namate = "matatrix"){
    std::cout<<namate<<" ("<<mat.GetSize()<<"x"<<mat.GetSize()<<"):\n";
    for(int i = 0; i < mat.GetSize(); i++){
        for(int j = 0; j < mat.GetSize(); j++){
            std::cout<<std::setw(4)<<mat.Get(i, j)<<" ";
        }
        std::cout<<"\n";
    }
}

void RunDequeManager(){
    DynamicArray<Deque<int>*> storage(0);
    auto AddToStorage = [&](Deque<int> dq){
        int cur = storage.GetSize();
        storage.Resize(cur + 1);
        storage.Set(cur, new Deque<int>(dq));
        std::cout<<"Added Deque at index ["<<cur<<"]\n";
    };

    while(true){
        std::cout<<"\n--- DEQUE STORAGE ---\n"
                 <<"1) Create  2) Delete  3) List  4) Push/Pop  5) Map/Where/Reduce\n"
                 <<"6) Concat  7) Merge   8) Sub   9) Contain  10) Sort  0) Back\n";
        int choice = SafeInput<int>("Choice: ");
        if(choice == 0){
            for(int i = 0; i < storage.GetSize(); i++){
                delete storage.Get(i);
            }
            return;
        }
        try{
            switch(choice){
                case 1:{
                    int count = SafeInput<int>("Count: ");
                    Deque<int> dq;
                    for(int i = 0; i < count; i++){
                        dq.PushBack(SafeInput<int>("Val: "));
                    }
                    AddToStorage(dq);
                    break;
                }
                case 2:{
                    int index = SafeInput<int>("Index: ");
                    delete storage.Get(index);
                    for(int i = index; i < storage.GetSize() - 1; i++){
                        storage.Set(i, storage.Get(i+1));
                    }
                    storage.Resize(storage.GetSize() - 1);
                    break;
                }
                case 3:{
                    for(int i = 0; i < storage.GetSize(); i++){
                        std::cout<<"["<<i<<"] Deque: ";
                        for(int j = 0; j < storage.Get(i)->GetSize(); j++){
                            std::cout<<storage.Get(i)->Get(j)<<" ";
                        }
                        std::cout<<"\n";
                    }
                    break;
                }
                case 4:{
                    int index = SafeInput<int>("index: ");
                    int oper = SafeInput<int>("1)PushF 2)PushB 3)PopF 4)PopB: ");
                    switch(oper){
                        case 1:{
                            storage.Get(index)->PushFront(SafeInput<int>("V: "));
                            break;
                        }
                        case 2:{
                            storage.Get(index)->PushBack(SafeInput<int>("V: "));
                            break;
                        }
                        case 3:{
                            std::cout<<storage.Get(index)->PopFront()<<"\n";
                            break;
                        }
                        case 4:{
                            std::cout<<storage.Get(index)->PopBack()<<"\n";
                            break;
                        }
                    }
                    break;
                }
                case 5:{
                    int index = SafeInput<int>("index: ");
                    int oper = SafeInput<int>("1)Map(^2) 2)Where(even) 3)Reduce(sum): ");
                    switch(oper){
                        case 1:{
                            AddToStorage(storage.Get(index)->Map(SquareFunc));
                            break;
                        }
                        case 2:{
                            AddToStorage(storage.Get(index)->Where(IsEven));
                            break;
                        }
                        case 3:{std::cout<<storage.Get(index)->Reduce(SumReducer, 0)<<"\n";
                            break;
                        }
                    }
                    break;
                }
                case 6:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    AddToStorage(storage.Get(index1)->Concat(*(storage.Get(index2))));
                    break;
                }
                case 7:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    AddToStorage(storage.Get(index1)->Merge(*(storage.Get(index2))));
                    break;
                }
                case 8:{
                    int index = SafeInput<int>("index: "), start = SafeInput<int>("Start: ");
                    int end = SafeInput<int>("End: ");
                    AddToStorage(storage.Get(index)->GetSubDeque(start, end));
                    break;
                }
                case 9:{
                    int index1 = SafeInput<int>("Main: ");
                    int index2 = SafeInput<int>("Sub: ");
                    std::cout<<(storage.Get(index1)->ContainSubDeque(*(storage.Get(index2))) ? "Yes\n" : "No\n");
                    break;
                }
                case 10:{
                    storage.Get(SafeInput<int>("index: "))->Sort(DescendingSort);
                    break;
                }
            }
        }
        catch(const std::exception& e){
            std::cout<<"Error: "<<e.what()<<"\n";
        }
    }
}

void RunVectorManager(){
    DynamicArray<Vector<int>*> storage(0);
    auto AddV = [&](Vector<int> v){
        int cur = storage.GetSize();
        storage.Resize(cur + 1);
        storage.Set(cur, new Vector<int>(v));
        std::cout<<"Vector added at ["<<cur<<"]\n";
    };

    while(true){
        std::cout<<"\n VECTOR STORAGE \n";
        std::cout<<"1)Create 2)Delete 3)List 4)Add 5)ScalarMult 6)DotProduct 7)Norm 0)Back\n";
        int choice = SafeInput<int>("Choice: ");
        if(choice == 0){
            for(int i = 0; i < storage.GetSize(); i++) delete storage.Get(i);
            return;
        }
        try{
            switch(choice){
                case 1:{
                    int dim = SafeInput<int>("Dim: "); 
                    Vector<int> v(dim);
                    for(int i = 0; i < dim; i++){
                        v(i) = SafeInput<int>("Val: ");
                    }
                    AddV(v);
                    break;
                }
                case 2:{
                    int index = SafeInput<int>("index: ");
                    delete storage.Get(index);
                    for(int i = index; i < storage.GetSize() - 1; i++){
                        storage.Set(i, storage.Get(i+1));
                    }
                    storage.Resize(storage.GetSize() - 1);
                    break;
                }
                case 3:{
                    for(int i = 0; i < storage.GetSize(); i++){
                        std::cout<<"["<<i<<"] ( ";
                        for(int j = 0; j < storage.Get(i)->GetDimension(); j++){
                            std::cout<<storage.Get(i)->Get(j)<<" ";
                        }
                        std::cout<<")\n";
                    }
                    break;
                }
                case 4:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    AddV(storage.Get(index1)->Add(*(storage.Get(index2))));
                    break;
                }
                case 5:{
                    int index = SafeInput<int>("index: ");
                    int scalar = SafeInput<int>("Scalar: ");
                    AddV(storage.Get(index)->MultyplyByScalar(scalar));
                    break;
                }
                case 6:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    std::cout<<"Result: "<<storage.Get(index1)->ScalarProductOfVectors(*(storage.Get(index2)))<<"\n";
                    break;
                }
                case 7: std::cout<<"Norm: "<<storage.Get(SafeInput<int>("index: "))->Norm()<<"\n";
                break;
            }
        }
        catch(const std::exception& e){
            std::cout<<"Error: "<<e.what()<<"\n";
        }
    }
}

void RunMatrixManager(){
    DynamicArray<SquareMatrix<int>*> storage(0);
    auto AddM = [&](SquareMatrix<int> m){
        int cur = storage.GetSize();
        storage.Resize(cur + 1);
        storage.Set(cur, new SquareMatrix<int>(m));
        std::cout<<"Matrix added at ["<<cur<<"]\n";
    };

    while(true){
        std::cout<<"\n MATRIX STORAGE \n";
        std::cout<<"1)Create 2)Delete 3)List 4)Add 5)Mult 6)SwapRows 7)SwapCols 8)RowMult 9)Norm 0)Back\n";
        int choice = SafeInput<int>("Choice: ");
        if(choice == 0){
            for(int i = 0; i < storage.GetSize(); i++){
                delete storage.Get(i);
            }
            return;
        }
        try{
            switch(choice){
                case 1:{
                    int size = SafeInput<int>("Size: "); SquareMatrix<int> m(size);
                    for(int i = 0; i < size; i++) for(int j = 0; j < size; j++) m(i,j) = SafeInput<int>("Val: ");
                    AddM(m);
                    break;
                }
                case 3:{
                    for(int k = 0; k < storage.GetSize(); k++){
                        int stor = storage.Get(k)->GetSize();
                        std::cout<<"["<<k<<"]:\n";
                        for(int i = 0; i < stor; i++){
                            for(int j = 0; j < stor; j++){
                                std::cout<<storage.Get(k)->Get(i,j)<<" ";
                                std::cout<<"\n";
                            }
                        }
                    }
                    break;
                }
                case 4:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    AddM(storage.Get(index1)->Add(*(storage.Get(index2)))); 
                    break;
                }
                case 5:{
                    int index1 = SafeInput<int>("index1: ");
                    int index2 = SafeInput<int>("index2: ");
                    AddM(*(storage.Get(index1)) * *(storage.Get(index2)));
                    break;
                }
                case 6:{
                    int index = SafeInput<int>("index: ");
                    int r1 = SafeInput<int>("R1: ");
                    int r2 = SafeInput<int>("R2: ");
                    AddM(storage.Get(index)->SwapRows(r1, r2));
                    break;
                }
                case 8:{
                    int index = SafeInput<int>("index: ");
                    int r = SafeInput<int>("R: ");
                    int s = SafeInput<int>("S: ");
                    AddM(storage.Get(index)->MultiplyRowByScalar(r, s));
                    break;
                }
                case 9:{
                    std::cout<<"Norm: "<<storage.Get(SafeInput<int>("index: "))->Norm()<<"\n";
                    break;
                }
            }
        }
        catch(const std::exception& e){
            std::cout<<"Error: "<<e.what()<<"\n";
        }
    }
}

int main(){
    while(true){
        std::cout<<"\n SYSTEM MENU \n";
        std::cout<<"1) Deques\n2) Vectors\n3) Matrices\n0) Exit\n";
        int choice = SafeInput<int>("Select: ");
        switch(choice){
            case 0: return 0;
            case 1: RunDequeManager(); break;
            case 2: RunVectorManager(); break;
            case 3: RunMatrixManager(); break;
            default: std::cout<<"Invalid choice.\n";
        }
    }
    return 0;
}