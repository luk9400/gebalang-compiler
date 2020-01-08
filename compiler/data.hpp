#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "symbol.hpp"

class Data {
    private:
        std::unordered_map<std::string, std::shared_ptr<symbol>> sym_map;
    public:
        long long memory_offset;

        Data();

        bool check_context(std::string name);
        void put_symbol(std::string name);
        symbol* get_symbol(std::string name);

        void init_constant(std::string name, long long value);
        
        void print_symbols();
};
