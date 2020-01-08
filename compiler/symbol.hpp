#pragma once

#include <string>

typedef struct symbol{
    std::string name;
    long long offset;
    long long value;
    bool is_init;

    symbol(std::string name, long long offset) {
        this->name = name;
        this->offset = offset;
        this->is_init = false; 
    }
} symbol;