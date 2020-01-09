#pragma once

#include <string>
#include <cmath>

typedef struct symbol{
    std::string name;
    long long offset;
    long long value;
    bool is_init;
    bool is_const;
    bool is_array;
    bool is_array_cell;
    long long array_start;
    long long array_end;
    long long array_size;

    // variables and constants
    symbol(std::string name, long long offset, bool is_const) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = is_const;
        this->is_array = false;
        this->is_array_cell = false; 
    }

    // array cells
    symbol(std::string name, long long offset, bool is_const, bool is_array_cell) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = is_const;
        this->is_array = false;
        this->is_array_cell = is_array_cell; 
    }

    // arrays
    symbol(std::string name, long long offset, long long array_start, long long array_end) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = false;
        this->is_array = true;
        this->array_start = array_start;
        this->array_end = array_end;
        this->array_size = abs(array_start) + abs(array_end) + 1;
    }
} symbol;