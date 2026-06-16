#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "Exceptions.h"
#include "Generator.h"

template <typename T>
class LazyCache{
private:
    DynamicArray<T> items;
    int capacity;
    int count;
    int first_index;
    int first_slot;
public:
    LazyCache(int capacity = 50) : items(capacity), capacity(capacity), count(0), first_index(0), first_slot(0){}
    
    bool Contains(int index) const{
        return count > 0 && index >= first_index && index < first_index + count;
    }

    bool Forgotten(int index) const{
        return count > 0 && index < first_index;
    }

    int GetCount() const{
        return count;
    }

    T Get(int index) const{
        if(!Contains(index)) throw IndexOutOfRange();
        int shift = index - first_index;
        int slot = (first_slot + shift) % capacity;
        return items.Get(slot);
    }

    void Push(int index, const T& value){
        if(count == 0){
            first_index = index;
            first_slot = 0;
            items.Set(0, value);
            count = 1;
            return;
        }
        if(count < capacity){
            int slot = (first_slot + count) % capacity;
            items.Set(slot, value);
            count++;
            return;
        }
        items.Set(first_slot, value);
        first_slot = (first_slot + 1) % capacity;
        first_index++;
    }
};

template <typename T>
class LazySequence: public Sequence<T>{
private:
    enum class AccessStatus{
        Value, End, Forgotten
    };

    struct AccessResult{
        AccessStatus status;
        T value;
    };

    mutable LazyCache<T> cache;
    LazyGenerator<T>* generator;
    mutable int generated;
    mutable bool ended;
    mutable Ordinal length;

    AccessResult Access(int index) const{
        if(index < 0) return{AccessStatus::End, T()};
        if(cache.Contains(index)) return{AccessStatus::Value, cache.Get(index)};
        if(cache.Forgotten(index)) return{AccessStatus::Forgotten, T()};
        if(length.IsFinite() && index >= length.ToInt()) return{AccessStatus::End, T()};
        while(generated <= index){
            if(ended || generator == nullptr){
                return{AccessStatus::End, T()};
            }
            LazyResult<T> next = generator->Get(this, generated);
            if(!next.HasValue()){
                ended = true;
                length = Ordinal::Finite(generated);
                return{AccessStatus::End, T()};
            }
            cache.Push(generated, next.Value());
            generated++;
        }
        if(!cache.Contains(index)) return{AccessStatus::Forgotten, T()};
        return{AccessStatus::Value, cache.Get(index)};
    }
public:
    LazySequence() : cache(50), generator(nullptr), 
        generated(0), ended(true), length(Ordinal::Finite(0)){}

    LazySequence(int count, const T* items) : cache(50), 
        generator(new ArrayGenerator<T>(items, count)), 
        generated(0), ended(false), length(Ordinal::Finite(count)){}
    
    LazySequence(const Sequence<T>* sequence) : cache(50), 
        generator(new ArrayGenerator<T>(sequence)), 
        generated(0), ended(false), length(Ordinal::Finite(sequence->GetLength())){}

    LazySequence(LazyGenerator<T>* generator, Ordinal length = Ordinal::Omega(1,0), int cache_size = 50) :
        cache(cache_size), generator(generator), 
        generated(0), ended(false), length(length){}

    LazySequence(std::function<LazyResult<T>(int, const LazySequence<T>*)> rule, Ordinal length = Ordinal::Omega(1,0), int cache_size = 50) :
        cache(cache_size), generator(new RuleGenerator<T>(rule)),  
        generated(0), ended(false), length(length){}

    LazySequence(const LazySequence<T>& other) : cache(other.cache), 
        generator(other.generator == nullptr ? nullptr : other.generator->Clone()),  
        generated(other.generated),
        ended(other.ended), length(other.length){}

    ~LazySequence(){
        delete generator;
    }

    LazyResult<T> TryGet(const Ordinal& index) const{
        if(index.IsFinite()){
            return TryGet(index.ToInt());
        }
        if(generator == nullptr){
            return LazyResult<T>::Empty();
        }
        return generator->GetOrdinal(this, index);
    }

    LazyResult<T> TryGet(int index) const{
        AccessResult result = Access(index);
        if(result.status == AccessStatus::Forgotten) throw IndexOutOfRange();
        if(result.status == AccessStatus::End) return LazyResult<T>::Empty();
        return LazyResult<T>(result.value);
    }

    T Get(int index) const override{
        LazyResult<T> result = TryGet(index);
        if(!result.HasValue()) throw IndexOutOfRange();
        return result.Value();
    }

    T Get(const Ordinal& index) const{
        LazyResult<T> result = TryGet(index);
        if(!result.HasValue()) throw IndexOutOfRange();
        return result.Value();
    }

    T GetCached(int index) const{
        if(!cache.Contains(index)) throw IndexOutOfRange();
        return cache.Get(index);
    }

    T GetFirst() const override{
        return Get(0);
    }

    T GetLast() const override{
        if(!length.IsFinite()) throw IndexOutOfRange();
        if(length.ToInt() == 0) throw IndexOutOfRange();
        return Get(length.ToInt() - 1);
    }

    int GetLength() const override{
        if(!length.IsFinite()) return -1;
        return length.ToInt();
    }

    Ordinal GetOrdinalLength() const{
        return length;
    }

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Concat(const Sequence<T>* other) override;
    LazySequence<T>* Concat(const LazySequence<T>* other);
    Sequence<T>* GetSubsequence(const int start_index, const int end_index) override;
    LazySequence<T>* Take(int count) const;
    LazySequence<T>* InsertSequenceAt(const Sequence<T>* sequence, int index);
    LazySequence<T>* InsertSequenceAt(const LazySequence<T>* sequence, int index);
};

template <typename T>
class TakeGenerator: public LazyGenerator<T>{
private:
    int count;
    LazySequence<T> parent;
public:
    TakeGenerator(const LazySequence<T>& parent, int count) : count(count), parent(parent){}
    
    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < 0 || index >= count) return LazyResult<T>::Empty();
        return parent.TryGet(index);
    }

    LazyGenerator<T>* Clone() const override{
        return new TakeGenerator<T>(*this);
    }
};

template <typename T>
class SubSequenceGenerator : public LazyGenerator<T>{
private:
    int start_index;
    int length;
    LazySequence<T> parent;
public:
    SubSequenceGenerator(const LazySequence<T>& parent, int start_index, int length) : start_index(start_index), length(length),  parent(parent){}
    
    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < 0 || index >= length) return LazyResult<T>::Empty();
        return parent.TryGet(start_index + index);
    }

    LazyGenerator<T>* Clone() const override{
        return new SubSequenceGenerator<T>(*this);
    }
};

template <typename T>
class ConcatGenerator : public LazyGenerator<T>{
private:
    LazySequence<T> left;
    LazySequence<T> right;
public:
    ConcatGenerator(const LazySequence<T>& left, const LazySequence<T>& right) : left(left), right(right){}

    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < 0) return LazyResult<T>::Empty();
        Ordinal left_length = left.GetOrdinalLength();
        if(left_length.IsFinite()){
            int left_count = left_length.ToInt();
            if(index < left_count){
                return left.TryGet(index);
            }
            return right.TryGet(index - left_count);
        }
        return left.TryGet(index);
    }
    
    LazyResult<T> GetOrdinal(const LazySequence<T>* owner, const Ordinal& index) override{
        if(index.IsFinite()){
            return Get(owner, index.ToInt());
        }
        Ordinal left_length = left.GetOrdinalLength();
        if(left_length.IsFinite()){
            return right.TryGet(index);
        }
        if(index.GetOmegaCount() < left_length.GetOmegaCount()){
            return left.TryGet(index);
        }
        if(index.GetOmegaCount() == left_length.GetOmegaCount()){
            if(index.GetIndex() < left_length.GetIndex()){
                return left.TryGet(index);
            }
            return right.TryGet(Ordinal::Finite(index.GetIndex() - left_length.GetIndex()));
        }
        if(right.GetOrdinalLength().IsFinite()){
            int offset = index.GetIndex() - left_length.GetIndex();
            return right.TryGet(offset);
        }
        return right.TryGet(index.MoveLeft(left_length.GetOmegaCount()));
    }

    LazyGenerator<T>* Clone() const override{
        return new ConcatGenerator<T>(*this);
    }
};

template <typename T>
class InsertSequenceGenerator : public LazyGenerator<T>{
private:
    int position;
    LazySequence<T> base;
    LazySequence<T> inserted;
public:
    InsertSequenceGenerator(const LazySequence<T>& base, const LazySequence<T>& inserted, int position) : position(position), base(base), inserted(inserted){}

    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < position){
            return base.TryGet(index);
        }
        Ordinal inserted_length = inserted.GetOrdinalLength();
        if(inserted_length.IsFinite()){
            int inserted_count = inserted_length.ToInt();
            if(index < position + inserted_count){
                return inserted.TryGet(index - position);
            }
            return base.TryGet(index - inserted_count);
        }
        return inserted.TryGet(index - position);
    }
    
    LazyResult<T> GetOrdinal(const LazySequence<T>* owner, const Ordinal& index) override{
        if(index.IsFinite()){
            return Get(owner, index.ToInt());
        }
        Ordinal inserted_length = inserted.GetOrdinalLength();
        if(inserted_length.IsFinite()){
            return base.TryGet(index);
        }
        if(index.GetOmegaCount() < inserted_length.GetOmegaCount()){
            return inserted.TryGet(index);
        }
        if(index.GetOmegaCount() == inserted_length.GetOmegaCount()){
            if(index.GetIndex() < inserted_length.GetIndex()){
                return inserted.TryGet(index);
            }
            return base.TryGet(position + index.GetIndex() - inserted_length.GetIndex());
        }
        return base.TryGet(index.MoveLeft(inserted_length.GetOmegaCount()));
    }

    LazyGenerator<T>* Clone() const override{
        return new InsertSequenceGenerator<T>(*this);
    }
};

template <typename T>
Sequence<T>* LazySequence<T>::GetSubsequence(const int start_index, const int end_index){
    if(start_index < 0 || end_index < start_index) throw IndexOutOfRange();
    int length = end_index - start_index + 1;
    return new LazySequence<T>(new SubSequenceGenerator<T>(*this, start_index, length), Ordinal::Finite(length));
}

template <typename T>
LazySequence<T>* LazySequence<T>::Take(int count) const{
    if(count < 0) throw IndexOutOfRange();
    return new LazySequence<T>(new TakeGenerator<T>(*this, count), Ordinal::Finite(count));
}
    
template <typename T>
Sequence<T>* LazySequence<T>::Append(const T& item){
    LazySequence<T> one(1, &item);
    return Concat(&one);
}

template <typename T>
Sequence<T>* LazySequence<T>::Prepend(const T& item){
    LazySequence<T> one(1, &item);
    return one.Concat(this);
}

template <typename T>
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index){
    if(index < 0) throw IndexOutOfRange();
    LazySequence<T> one(1, &item);
    return InsertSequenceAt(&one, index);
}

template <typename T>
LazySequence<T>* LazySequence<T>::Concat(const LazySequence<T>* other){
    if(other == nullptr) throw NullPointer();
    Ordinal result_length = length + other->GetOrdinalLength();
    return new LazySequence<T>(
        new ConcatGenerator<T>(*this, *other),
        result_length
    );
}

template <typename T>
Sequence<T>* LazySequence<T>::Concat(const Sequence<T>* other){
    if(other == nullptr) throw NullPointer();
    const LazySequence<T>* lazy_other = dynamic_cast<const LazySequence<T>*>(other);
    if(lazy_other != nullptr){
        return Concat(lazy_other);
    }
    LazySequence<T> wrapped_other(other);
    return Concat(&wrapped_other);
}

template <typename T>
LazySequence<T>* LazySequence<T>::InsertSequenceAt(const LazySequence<T>* sequence, int index){
    if(sequence == nullptr) throw NullPointer();
    if(index < 0) throw IndexOutOfRange();
    if(length.IsFinite() && index > length.ToInt()){
        throw IndexOutOfRange();
    }
    Ordinal result_length = length + sequence->GetOrdinalLength();
    return new LazySequence<T>(
        new InsertSequenceGenerator<T>(*this, *sequence, index),
        result_length
    );
}

template <typename T>
LazySequence<T>* LazySequence<T>::InsertSequenceAt(const Sequence<T>* sequence, int index){
    if(sequence == nullptr) throw NullPointer();
    const LazySequence<T>* lazy_seq = dynamic_cast<const LazySequence<T>*>(sequence);
    if(lazy_seq != nullptr){
        return InsertSequenceAt(lazy_seq, index);
    }
    LazySequence<T>* wrapped_seq(sequence);
    return InsertSequenceAt(&wrapped_seq, index);
}