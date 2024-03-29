#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

#include "data.hpp"
#include "symbol.hpp"

Data::Data() {
    this->memory_offset = 1;
    this->put_symbol("1", true);
    this->put_symbol("-1", true);
    this->put_symbol("A", false);
    this->put_symbol("B", false);
    this->put_symbol("C", false);
    this->put_symbol("D", false);
    this->put_symbol("E", false);
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
        throw std::string(name + " - variable not defined");
    }

    return this->sym_map[name].get();
}

void Data::put_symbol(std::string name, bool is_const) {
    if (this->check_context(name)) {
        throw std::string(name + " - already defined");
    }

    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, this->memory_offset, is_const);
    this->memory_offset++;
    this->sym_map[name] = sym;
}

void Data::put_array(std::string name, long long array_start, long long array_end) {
    if (this->check_context(name)) {
        throw std::string(name + " - already defined");
    }

    if (array_end < array_start) {
        throw std::string(name + " - bad range");
    }

    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, this->memory_offset, array_start, array_end);

    this->memory_offset += array_end - array_start + 1;
    this->sym_map[name] = sym;
}

void Data::put_array_cell(std::string name, long long offset) {
    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, offset, true, false);
    this->sym_map[name] = sym;
}

void Data::put_addr_cell(std::string name, long long offset) {
    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, offset, false, true);
    this->sym_map[name] = sym;
}

void Data::put_iterator(std::string name) {
    if (this->check_context(name)) {
        throw std::string("Redeclarating iterator - " + name);
    }

    std::shared_ptr<symbol> sym = std::make_shared<symbol>(name, this->memory_offset);
    this->memory_offset++;
    this->sym_map[name] = sym;
}

void Data::remove_iterator(std::string name) {
    sym_map.erase(name);
}

void Data::init_constant(std::string name, long long value) {
    symbol* sym = this->get_symbol(name);
    sym->is_init = true;
    sym->value = value;
}

void Data::print_symbols() {
    std::unordered_map<std::string, std::shared_ptr<symbol>>::iterator it;

    for (it = this->sym_map.begin(); it != this->sym_map.end(); it++) {
        std::cout << "Name: " << it->second->name << " Offset: " << it->second->offset << std::endl;
    }
}