#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

#include "data.hpp"
#include "symbol.hpp"

Data::Data() {
    this->memory_offset = 1;
}

bool Data::check_context(std::string name) {
    std::unordered_map<std::string, std::shared_ptr<symbol>>::iterator it = this->sym_map.find(name);

    if (it != sym_map.end()) {
        return true;
    } else {
        return false;
    }
}

symbol* Data::get_symbol(std::string name) {
    if (!this->check_context(name)) {
        std::cerr << name << " - variable not defined" << std::endl;
        return nullptr;
    }

    return this->sym_map[name].get();
}

void Data::put_symbol(std::string name) {
    if (this->check_context(name)) {
        std::cerr << name << " - already defined" << std::endl;
        return;
    }

    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, this->memory_offset);
    this->memory_offset++;
    this->sym_map[name] = sym;
}

void Data::print_symbols() {

    std::unordered_map<std::string, std::shared_ptr<symbol>>::iterator it;

    for (it = this->sym_map.begin(); it != this->sym_map.end(); it++) {
        std::cout << "Name: " << it->second->name << " Offset: " << it->second->offset << std::endl;
    }
}