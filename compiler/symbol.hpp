#pragma once

#include <string>

typedef struct symbol{
    std::string name;
    long long offset;
    long long value;
    bool is_init;
    bool is_const;

    symbol(std::string name, long long offset, bool is_const) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = is_const; 
    }
} symbol;