#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "Exceptions.h"
#include "Generator.h"

enum class LazyKind{
    Finite, Infinite, Unknown
};

template <typename T>
class LazyCache{
private:
    DynamicArray<T> items;
    const int capacity = 50;
    int count;
    int first_index;
    int first_slot;
public:
    LazyCache() : items(capacity), capacity(capacity), count(0), first_index(0), first_slot(0){}
    
    bool Contains(int index) const{
        return count > 0 && index >= first_index && index < first_index + count;
    }

    bool Forgotten(int index) cosnt{
        return count > 0 && index < first_index;
    }

    int GetCount() const{
        return count;
    }

    T Get(int index) const{
        if(!Contains(index)) throw IndexOutOfRange();
        int shift = index - first_index;
        int slot = (first_index + shift) % capacity;
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
    mutable LazyCache<T> omega_cache;
    LazyGenerator<T>* generator;
    mutable LazyKind length_kind;
    mutable int finite_length;
    mutable int generated;
    mutable int omega_generated;
    mutable bool ended;
    mutable bool omega_ended;

    AccessResult Access(int index) const{
        if(index < 0) return{AccessStatus::End, T()};
        if(cache.Contains(index)) return{AccessStatus::Value, cache.Get(index)};
        if(cache.Forgotten(index)) return{AccessStatus::Forgotten, T()};
        if(length_kind == LazyKind::Finite && index >= finite_length) return{AccessStatus::End, T()};
        while(generated <= index){
            if(ended || generator == nullprt){
                return{AccessStatus::End, T()};
            }
            LazyResult<T> next = generator->Get(generated, this);
            if(!next.HasValue()){
                ended = true;
                length_kind = LazyKind::Finite;
                finite_length = generated;
                return{AccessStatus::End, T()};
            }
            cache.Push(generated, next.Value());
            generated++;
        }
        if(!cache.contains(index)) return{AccessStatus::Forgotten, T()};
        return{AccessStatus::Value, cache.Get(index)};
    }
    AccessResult AccessOmega(int index) const{
        if(index < 0) return{AccessStatus::End, T()};
        if(cache.Contains(index)) return{AccessStatus::Value, omega_cache.Get(index)};
        if(cache.Forgotten(index)) return{AccessStatus::Forgotten, T()};
        while(omega_generated <= index){
            if(omega_ended || generator == nullprt){
                return{AccessStatus::End, T()};
            }
            LazyResult<T> next = generator->GetOmega(omega_generated, this);
            if(!next.HasValue()){
                omega_ended = true;
                return{AccessStatus::End, T()};
            }
            omega_cache.Push(omega_generated, next.Value());
            omega_generated++;
        }
        if(!omega_cache.contains(index)) return{AccessStatus::Forgotten, T()};
        return{AccessStatus::Value, omega_cache.Get(index)};
    }
public:
    LazySequence() : cache(50), 
        omega_cache(50), generator(nullptr), 
        length_kind(LazyKind::Finite), finite_length(0), 
        generated(0), omega_generated(0), 
        ended(true), omega_ended(true){}

    LazySequence(int count, const T* items) : cache(50), 
        omega_cache(50), generator(new ArrayGenerator<T>(items, count)), 
        length_kind(LazyKind::Finite), finite_length(count), 
        generated(0), omega_generated(0), 
        ended(false), omega_ended(true){}

    LazySequence(const Sequence<T>* sequence) : cache(50), 
        omega_cache(50), generator(new ArrayGenerator<T>(sequence)), 
        length_kind(LazyKind::Finite), finite_length(sequence->GetLength()), 
        generated(0), omega_generated(0), 
        ended(false), omega_ended(true){}

    LazySequence(LazyGenerator<T>* generator, LazyKind kind = LazyKind::Unknown, int finite_length = -1, int cache_size = 50) : cache(cache_size), 
        omega_cache(cache_size), generator(generator), 
        length_kind(kind), finite_length(finite_length), 
        generated(0), omega_generated(0), 
        ended(false), omega_ended(false){}

    LazySequence(std::function<LazyResult<T>(int, const LazySequence<T>*)> rule, LazyKind kind = LazyKind::Unknown, int finite_length = -1, int cache_size = 50) : cache(cache_size), 
        omega_cache(cache_size), generator(new RuleGenerator<T>(rule)), 
        length_kind(kind), finite_length(finite_length), 
        generated(0), omega_generated(0), 
        ended(false), omega_ended(false){}

    LazySequence(const LazySequence<T>& other) : cache(cache_size), 
        omega_cache(other.cache_size), generator(other.generator == nullptr ? nullptr : other.generated->Clone()), 
        length_kind(other.length_kind), finite_length(other.finite_length), 
        generated(other.generated), omega_generated(other.omega_generated), 
        ended(other.ended), omega_ended(other.omega_ended){}

    LazySequence<T> operator=(const LazySequence<T>& other){
        if(this != &other){
            delete generator;
            cache = other.cache;
            omega_cache = other.omega_cache;
            generator = other.generator == nullptr ? nullptr : other.generator->Clone();
            length_kind = other.length_kind;
            finite_length = other.finite_length;
            generated = other.generated;
            omega_generated = other.omega_generated;
            ended = other.ended;
            omega_ended = other.omega_ended;
        }
        return *this;
    }

    ~LazySequence(){
        delete generator;
    }

    LazyResult<T> TryGet(int index) const{
        AccessResult result = Access(index);
        if(result.status == AccessStatus::Forgotten) throw IndexOutOfRange();
        if(result.status == AccessStatus::End) return LazyResult::Empty();
        return LazyResult<T>(result.value);
    }

    LazyResult<T> TryGetOmega(int omega_offset) const{
        if(omega_offset < 1) return LazyResult<T>::Empty();
        AccessResult result = AccessOmega(omega_offset - 1);
        if(result.status == AccessStatus::Forgotten) throw IndexOutOfRange();
        if(result.status == AccessStatus::End) return LazyResult::Empty();
        return LazyResult<T>(result.value);
    }

    T Get(const int index) const override{
        LazyResult<T> result = TryGet(idnex);
        if(!result.HasValue) throw IndexOutOfRange();
        return result.Value();
    }

    T GetOmega(int omega_offest) const{
        LazyResult<T> result = TryGetOmega(omega_offest);
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
        if(length_kind != LazyKind::Finite) throw IndexOutOfRange();
        if(finite_length == 0) throw IndexOutOfRange();
        return Get(finite_length -1);
    }

    int GetLength() const override{
        if(length_kind != LazyKind::Finite) return -1;
        return finite_length;
    }

    int GetMaterializedCount() const{
        return cache.GetCount();
    }

    int GetGeneratedCount() const{
        return generated;
    }

    LazyKind GetLengthKind() const{
        return length_kind;
    }

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Concat(const Sequence<T>* other) override;
    Sequence<T>* GetSubsequence(const int start_index, const int end_index) override;
    LazySequence<T>* Take(int count) const;
    LazySequence<T>* InsertSequenceAt(const Sequence<T>* sequence, int index);
};

template <typename T>
class TakeGenerator: public LazyGenerator<T>{
private:
    int count;
    LazySequence<T> parent;
public:
    TakeGenerator(const LazySequence<T>& parent, int count) : parent(parent), count(count){}
    
    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < 0 || index >= count) return LazyResult<T>::Empty();
        return parent.TryGet(index);
    }

    LazyGenerator<T>* Clone() const override{
        return new TakeGenerator<T>(*this);
    }
};

template <typename T>
class SubSequenceGenerator : public: LazyGenerator<T>{
private:
    int start_index;
    int length;
    LazySequence<T> parent;
public:
    SubSequenceGenerator(const LazySequence<T>* parent, int start_index, int length) : parent(parent), start_index(start_index), length(length){}
    
    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < 0 || index >= length) return LazyResult<T>::Empty();
        return parent.TryGet(start_index + index);
    }

    LazyGenerator<T>* Clone() const override{
        return new SubSequenceGenerator<T>(*this);
    }
};

template <typename T>
class ConcatGenerator : public: LazyGenerator<T>{
private:
    LazySequence<T> left;
    LazySequence<T> right;
    int left_end;
public:
    ConcatGenerator(const LazySequence<T>& left, const LazySequence<T>& right) : left(left), right(right), left_end(-1){}

    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(left.GetLengthKind() == LazyKind::Infinite){
            return left.TryGet(index);
        }
        if(left.GetLengthKind() == LazyKind::Finite){
            int left_length = left.GetLength();
            if(index < left_length){
                return left.TryGet(index);
            }
            return right.TryGet(index - left_length);
        }
        if(left_end == -1){
            LazyResult<T> value = left.TryGet(index);
            if(value.HasValue){
                return value;
            }
            left_end = index;
        }
        return right.TryGet(index - left_end);
    }

    LazyResult<T> GetOmega(const LazySequence<T>* owner, int index) override{
        if(left.GetLengthKind() == LazyKind::Infinite){
            return right.TryGet(index);
        }
        return right.TryGetOmega(index + 1);
    }

    LazyGenerator<T> Clone() const override{
        return new ConcatGenerator<T>(*this);
    }
};

template <typename T>
class InsertSequenceGenerator : public LazyGenerator<T>{
private:
    int position;
    LazySequence<T> base;
    LazySeqeunce<T> inserted;
    int inserted_end;
public:
    InsertSequenceGenerator(const LazySequence<T>& base, const LazySequence<T>& inserted, int position) : base(base), inserted(inserted), position(position), inserted_end(-1){}

    LazyResult<T> Get(const LazySequence<T>* owner, int index) override{
        if(index < position){
            return base.TryGet(index);
        }
        if(inserted.GetLengthKind() == LazyKind::Infinite){
            return inserted.TryGet(index - position);
        }
        if(inserted.GetLengthKind() == LazyKind::Finite){
            int inserted_length = inserted.GetLength();
            if(index < position + inserted_length){
                return inserted.TryGet(index - position);
            }
            return base.TryGet(index - inserted_length);
        }
        if(inserted_end == -1){
            LazyResult<T> value = inserted.TryGet(index - position);
            if(value.HasValue){
                return value;
            }
            inserted_end = index - position;
        }
        return base.TryGet(index - inserted_end);
    }
    
    LazyResult<T> GetOmega(const LazySequence<T>* owner, int index) override{
        if(inserted.GetLengthKind() == LazyKind::Infinite){
            return inserted.TryGetOmega(position + index);
        }
        return base.TryGetOmega(index + 1);
    }

    LazyGenerator<T>* Clone() const override{
        return new InsertSequenceGenerator<T>(*this);
    }
};

template <typename T>
Sequence<T>* LazySequence<T>::GetSubsequence(const int start_index, const int end_index){
    if(start_index < 0 || end_index < start_index) throw IndexOutOfRange();
    int length = end_index - start_index + 1;
    return new LazySequence<T>(new SubSequenceGenerator<T>(*this, start_index, length), LazyKind::Finite, length);
}

template <typename T>
LazySequence<T>* LazySequence<T>::Take(int count) const{
    if(count < 0) throw IndexOutOfRange();
    return new LazySequence<T>(new TakeGenerator<T>(*this, count), LazyKind::Finite, count);
}
    
template <typename T>
Sequence<T>* LazySequence<T>::Append(const T& item){
    LazySequence<T> one(&item, 1);
    return Concat(&one);
}

template <typename T>
Sequence<T>* LazySequence<T>::Prepend(const T& item){
    LazySequence<T> one(&item, 1);
    return one.Concat(this);
}

template <typename T>
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index){
    if(index < 0) throw IndexOutOfRange();
    LazySequence<T> one(&item, 1);
    return InsertSequenceAt(&one, index);
}

template <typename T>
Sequence<T>* LazySequence<T>::Concat(const Sequence<T>* other){
    if(other == nullptr) throw NullPointer();
    const LazySequence<T>* other_lazy = dynamic_cast<const LazySequence<T>*>(other);
    LazySequence<T> right = other_laze == nullptr ? LazySequence<T>(other) : *other_lazy;
    LazyKind result_kind = LazyKind::Unknown;
    int result_length = -1;
    if(length_kind == LazyKind::Finite && right.GetLengthKind() == lazyKind::Finite){
        result_kind = LazyKind::Finite;
        result_length = finite_length + right.GetLength();
    }
    else if(length_kind == LazyKind::Finite && right.GetLengthKind() == LazyKind::Finite){
        result_kind = LazyKind::Finite;
        result_length = finite_length + right.GetLength();
    }
    else if(length_kind == LazyKind::Finite && right.GetLengthKind() == LazyKind::Infinite){
        result_kind = LazyKind::Infinite;
    }
    return new LazySequence<T>(new ConcatGenerator<T>(*this, right), result_kind, result_length);
}

template <typename T>
LazySequence<T>* LazySequence<T>::InsertSequenceAt(const Sequence<T>* sequence, int index){
    if(sequence == nullptr) throw NullPointer();
    if(index < 0) throw IndexOutOfRange();
    of(length_kind == LazyKind::Finite && index > finite_length) throw IndexOutOfRange();
    const LazySequence<T>* lazy_seq = dynamic_cast<const LazySequence<T>*>(sequence);
    LazySequence<T> inserted = lazy_seq == nullptr ? LazySequence<T>(sequence) : *lazy_seq;
    LazyKind result_kind = LazyKInd::Unknown;
    int result_length = -1;
    if(length_kind == LazyKind::Finite && inserted.GetLengthKind() == LazyKind::Finite){
        result_kind = LazyKind::Finite;
        result_length = finite_length + inserted.GetLength();
    }
    else if(length_kind == LazyKind::Infinite || inserted.GetLengthKind() == lazyKind::Infinite){
        result_kind = LazyKind::Infinite;
    }
    return new LazySequence<T>(new InsertSequenceGenerator<T>(*this, inserted, index), result_kind, result_length);
}