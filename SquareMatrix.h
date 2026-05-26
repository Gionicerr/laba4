#pragma once
#include "MutableArraySequence.h"
#include "Exceptions.h"

template <typename T>
class SquareMatrix{
private:
    const int size;
    MutableArraySequence<T> items;
    
    int GetIndex(int row, int column) const{
        if(row < 0 || row >= size || column < 0 || column >= size) throw IndexOutOfRange();
        return row * size + column;
    }

    void CheckSameSize(const SquareMatrix<T>& other) const{
        if(size != other.GetSize()) throw InvalidSize();
    }
public:
    SquareMatrix() : size(1), items(){}

    SquareMatrix(int matrix_size) : size(matrix_size), items(matrix_size * matrix_size){
        if(matrix_size < 0) throw InvalidSize();
    }

    SquareMatrix(T* data, int matrix_size) : size(matrix_size), items(data, matrix_size * matrix_size){
        if(matrix_size < 0) throw InvalidSize();
    }

    int GetSize() const{
        return size;
    }

    T Get(int row, int column) const{
        return items.Get(GetIndex(row,column));
    }

    T& operator()(int row, int column){
        return items(GetIndex(row, column));
    }

    const T& operator()(int row, int column) const{
        return items(GetIndex(row, column));
    }

    SquareMatrix<T> operator+(const SquareMatrix<T>& other) const{
        return Add(other);
    }

    SquareMatrix<T> operator*(const T& scalar) const{
        return MultiplyByScalar(scalar);
    }

    SquareMatrix<T> operator*(const SquareMatrix<T>& other) const{
        CheckSameSize(other);
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                T value = T();
                for(int k = 0; k < size; k++){
                    value = value + Get(row, k) * other.Get(k, column);
                }
                result(row, column) = value;
            }
        }
        return result;
    }

    SquareMatrix<T> Add(const SquareMatrix<T>& other) const{
        CheckSameSize(other);
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                result(row, column) = Get(row, column) + other.Get(row, column);
            }
        }
        return result;
    }

    SquareMatrix<T> MultiplyByScalar(const T& scalar) const{
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                result(row, column) = Get(row, column) * scalar;
            }
        }
        return result;
    }

    double Norm() const{
        double result = 0.0;
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                double value = Get(row, column);
                result += value * value;
            }
        }
        return std::sqrt(result);
    }

    SquareMatrix<T> SwapRows(int first_row, int second_row) const{
        if(first_row < 0 || second_row < 0 || first_row >= size || second_row >= size) throw IndexOutOfRange();
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                if(row == first_row){
                    result(row, column) = Get(second_row, column);
                }
                else if(row == second_row){
                    result(row, column) = Get(first_row, column);
                }
                else{
                    result(row, column) = Get(row, column);
                }
            }
        }
        return result;
    }

    SquareMatrix<T> SwapColumns(int first_column, int second_column) const{
        if(first_column < 0 || second_column < 0 || first_column >= size || second_column >= size) throw IndexOutOfRange();
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                if(column == first_column){
                    result(row, column) = Get(row, second_column);
                }
                else if(column == second_column){
                    result(row, column) = Get(row, first_column);
                }
                else{
                    result(row, column) = Get(row, column);
                }
            }
        }
        return result;
    }

    SquareMatrix<T> MultiplyRowByScalar(int row_index, const T& scalar) const{
        if(row_index < 0 || row_index >= size) throw IndexOutOfRange();
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                if(row == row_index){
                    result(row, column) = Get(row, column) * scalar;
                }
                else{
                    result(row, column) = Get(row, column);
                }
            }
        }
        return result;
    }

    SquareMatrix<T> MultiplyColumnByScalar(int column_index, const T& scalar) const{
        if(column_index < 0 || column_index >= size) throw IndexOutOfRange();
        SquareMatrix<T> result(size);
        for(int row = 0; row < size; row++){
            for(int column = 0; column < size; column++){
                if(column == column_index){
                    result(row, column) = Get(row, column) * scalar;
                }
                else{
                    result(row, column) = Get(row, column);
                }
            }
        }
        return result;
    }
};