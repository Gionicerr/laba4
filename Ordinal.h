#pragma once
#include "Exceptions.h"

class Ordinal{
private:
    int omega_count;
    int index;
public:
    Ordinal(): omega_count(0), index(0){}

    static Ordinal Finite(int value){
        if(value < 0) throw IndexOutOfRange();
        Ordinal result;
        result.omega_count = 0;
        result.index = value;
        return result;
    }

    static Ordinal Omega(int omega_count, int index){
        if(omega_count < 1 || index < 0) throw IndexOutOfRange();
        Ordinal result;
        result.omega_count = omega_count;
        result.index = index;
        return result;
    }

    static Ordinal Length(int omega_count, int index){
        if(omega_count < 0 || index < 0) throw InvalidSize();
        Ordinal result;
        result.omega_count = omega_count;
        result.index = index;
        return result;
    }

    bool IsFinite() const{
        return omega_count == 0;
    }
    bool IsOmega() const{
        return omega_count>0;
    }
    int GetOmegaCount() const{
        return omega_count;
    }
    int GetIndex() const{
        return index;
    }

    int ToInt() const{
        if(!IsFinite()) throw InvalidSize();
        return index;
    }

    Ordinal MoveLeft(int count) const{
        if(count < 0 ||count > omega_count) throw IndexOutOfRange();
        if(count == omega_count){
            return Ordinal::Finite(index);
        }
        return Ordinal::Length(omega_count - count, index);
    }

    Ordinal operator+(const Ordinal& other) const{
        if(other.omega_count > omega_count){
            return other;
        }
        if(other.omega_count == omega_count){
            if(other.IsFinite()){
                return Ordinal::Finite(index + other.index);
            }
            return Ordinal::Omega(omega_count, index + other.index);
        }
        return Ordinal::Length(omega_count, index);
    }
};