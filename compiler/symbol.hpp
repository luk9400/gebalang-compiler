#pragma once

#include <string>

typedef struct symbol {
    std::string name;
    long long offset;
    long long value;
    bool is_init;
    bool is_const;
    bool is_array;
    bool is_array_cell;
    bool is_addr_cell;
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
        this->is_addr_cell = false;
    }

    // array cells
    symbol(std::string name, long long offset, bool is_array_cell, bool is_addr_cell) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = false;
        this->is_array = false;
        this->is_array_cell = is_array_cell;
        this->is_addr_cell = is_addr_cell;
    }

    // arrays
    symbol(std::string name, long long offset, long long array_start, long long array_end) {
        this->name = name;
        this->offset = offset;
        this->is_init = false;
        this->is_const = false;
        this->is_array = true;
        this->is_array_cell = false;
        this->is_addr_cell = false;
        this->array_start = array_start;
        this->array_end = array_end;
        this->array_size = array_end - array_start + 1;
    }
} symbol;