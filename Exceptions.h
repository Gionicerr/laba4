#pragma once

class IndexOutOfRange: public std::exception{
public:
    const char* what() const noexcept override{
        return "IndexOutOfRange: Invalid Index";
    }
};
class InvalidSize: public std::exception{
public:
    const char* what() const noexcept override{
        return "InvalidSize: Size cannot be negative";
    }
}; 
class NullPointer: public std::exception{
public:
    const char* what() const noexcept override{
        return "NullPointer: Data pointer is null";
    }
};
