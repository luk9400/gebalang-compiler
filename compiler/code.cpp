#include <memory>
#include <string>
#include <vector>

#include "code.hpp"
#include "data.hpp"
#include "symbol.hpp"

Code::Code(std::shared_ptr<Data> data) {
    this->pc = 0;
    this->data = data;
}

std::vector<std::string> Code::get_code() {
    return this->code;
}

void Code::end_code() {
    this->code.push_back("HALT");
    this->pc++;
}

// COMMANDS

void Code::assign(symbol* var) {
    this->code.push_back("STORE " + std::to_string(var->offset));
    var->is_init = true;
    this->pc++;
}

void Code::write(symbol* sym) {
    this->check_init(sym);

    this->code.push_back("LOAD " + std::to_string(sym->offset));
    this->code.push_back("PUT");
    this->pc += 2;
}

void Code::read(symbol* sym) {
    this->code.push_back("GET");
    this->code.push_back("STORE " + std::to_string(sym->offset));
    sym->is_init = true;
    this->pc += 2;
}

// EXPRESSIONS

void Code::load_value(symbol* sym) {
    this->check_init(sym);

    this->code.push_back("LOAD " + std::to_string(sym->offset));
    this->pc++;
}

void Code::plus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    this->code.push_back("LOAD " + std::to_string(a->offset));
    this->code.push_back("ADD " + std::to_string(b->offset));
    this->pc += 2;
}

void Code::minus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);
    
    this->code.push_back("LOAD " + std::to_string(a->offset));
    this->code.push_back("SUB " + std::to_string(b->offset));
    this->pc += 2;
}

// VALUES & PIDs

symbol* Code::get_num(long long num) {
    // get constant if its declared
    if (this->data->check_context(std::to_string(num))) {
        return this->data->get_symbol(std::to_string(num));
    } else {  // declare const
        this->data->put_symbol(std::to_string(num), true);

        return this->data->get_symbol(std::to_string(num));
    }
}

symbol* Code::pidentifier(std::string name) {
    symbol* sym = this->data->get_symbol(name);
    if (sym != nullptr) {
        return sym;
    } else {
        throw std::string(name + " - symbol does not exist");
    }
}

// MISC

void Code::generate_constant(long long value, long long offset) {
    this->code.push_back("SUB 0");
    if (value > 0) {
        for (long long i = 0; i < value; i++) {
            this->code.push_back("INC");
            this->pc++;
        }
    }
    if (value < 0) {
        for (long long i = 0; i < -value; i++) {
            this->code.push_back("DEC");
            this->pc++;
        }
    }

    this->code.push_back("STORE " + std::to_string(offset));
    this->pc += 2;
}

void Code::init_const(symbol* sym) {
    this->data->init_constant(sym->name, std::stoll(sym->name));
    this->generate_constant(std::stoll(sym->name), sym->offset);
}

void Code::check_init(symbol* sym) {
    if (!sym->is_init) {
        if (sym->is_const) {
            this->init_const(sym);
        } else {
            throw std::string(sym->name + " - symbol is not initialized");
        }
    }
}