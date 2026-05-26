#pragma once

template <typename T>
class Sequence{
public:
    virtual T Get(const int index) const = 0;
    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual int GetLength() const = 0;
    virtual Sequence<T>* Append(const T& value) = 0;
    virtual Sequence<T>* Prepend(const T& value) = 0;
    virtual Sequence<T>* InsertAt(const T& value, int index) = 0;
    virtual Sequence<T>* Concat(const Sequence<T>* other) = 0;
    virtual Sequence<T>* GetSubsequence(const int start_index, const int end_index) = 0;
    virtual ~Sequence() {}
};