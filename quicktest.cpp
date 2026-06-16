#include <iostream>
#include <exception>
#include "LazySequence.h"
#include "MutableArraySequence.h"

LazyResult<int> rule(int index, const LazySequence<int>* seq){
    return LazyResult<int>(index * 2);
}

LazyResult<int> rule2(int index, const LazySequence<int>* seq){
    return LazyResult<int>(index * 5);
}
int main(){
    
    LazySequence<int> res(rule, Ordinal::Omega(1,0));
    std::cout<<res.Get(10)<<"\n";

    LazySequence<int> res2(rule2, Ordinal::Omega(1,0));
    LazySequence<int>* conc = res.Concat(&res2);
    std::cout<<conc->Get(4)<<"\n";
    std::cout<<conc->Get(Ordinal::Omega(1,3))<<"\n";

    LazySequence<int> sub = res.GetSubsequence(1,5);
    for(int i = 0; i<sub.GetLength(); i++){
        std::cout<<sub.Get(i)<<"\n";
    }

    MutableArraySequence<int> ab(2);
    ab.Append(1);
    ab.Append(228);
    LazySequence<int> abl(&ab);
    LazySequence<int>* abcon = res.Concat(&abl);
    std::cout<<abcon->Get(Ordinal::Omega(1,1))<<"/n";
    std::cout<<res.Get(100)<<"\n";
}