#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "code.hpp"
#include "data.hpp"
#include "symbol.hpp"
#include "labels.hpp"

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
    this->store(var);
    var->is_init = true;
    this->pc++;
}

void Code::if_block(cond_label* label) {
    this->code[label->go_to] += std::to_string(this->pc);
}

void Code::write(symbol* sym) {
    this->check_init(sym);

    this->load(sym);
    this->code.push_back("PUT");
    this->pc += 2;
}

void Code::read(symbol* sym) {
    this->code.push_back("GET");
    this->store(sym);
    sym->is_init = true;
    this->pc += 2;
}

// EXPRESSIONS

void Code::load_value(symbol* sym) {
    this->check_init(sym);

    this->load(sym);
    this->pc++;
}

void Code::plus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);

    if (a->is_addr_cell && b->is_addr_cell) {
        this->load(a);
        this->code.push_back("STORE " + std::to_string(this->data->memory_offset));
        this->load(b);
        this->ADD(this->data->memory_offset);
        this->pc += 4;
    } else if (b->is_addr_cell && !a->is_addr_cell) {
        this->load(b);
        this->ADD(a->offset);
        this->pc += 2;
    } else {
        this->load(a);
        this->ADD(b->offset);
        this->pc += 2;
    }
}

void Code::minus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);

    if (b->is_addr_cell) {
        this->load(b);
        this->code.push_back("STORE " + std::to_string(this->data->memory_offset));
        this->load(a);
        this->SUB(this->data->memory_offset);
        this->pc += 4;
    } else {
        this->load(a);
        this->SUB(b->offset);
        this->pc += 2;
    }
}

// CONDITIONS

cond_label* Code::eq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->code.push_back("JZERO " + std::to_string(this->pc + 2));
    this->code.push_back("JUMP ");
    this->pc += 2;
    long long addr = this->pc - 1;

    std::shared_ptr<cond_label> label = std::make_shared<cond_label>(start, addr);
    return label.get();
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

symbol* Code::array_num_pidentifier(std::string name, long long num) {
    symbol* sym = this->data->get_symbol(name);
    if (sym != nullptr) {
        if (sym->array_start <= num && sym->array_end >= num) {
            long long offset = num - sym->array_start + sym->offset;

            if (!this->data->check_context(name + std::to_string(num))) {
                this->data->put_array_cell(name + std::to_string(num), offset);
            }
            this->data->put_array_cell(name + std::to_string(num), offset);
            symbol* ret_sym = this->data->get_symbol(name + std::to_string(num));

            return ret_sym;
        } else {
            throw std::string(name + " - index out of boundry");
        }
    } else {
        throw std::string(name + " - array does not exist");
    }
}

symbol* Code::array_pid_pidentifier(std::string name, std::string pid_name) {
    symbol* array = this->data->get_symbol(name);
    symbol* var = this->data->get_symbol(pid_name);

    if (array != nullptr && var != nullptr) {
        // init constants
        symbol* array_start = this->get_num(array->array_start);
        this->check_init(array_start);
        symbol* array_offset = this->get_num(array->offset);
        this->check_init(array_offset);

        // calculate address of a cell
        this->code.push_back("LOAD " + std::to_string(var->offset));
        this->code.push_back("SUB " + std::to_string(array_start->offset));
        this->code.push_back("ADD " + std::to_string(array_offset->offset));

        // save address
        this->data->put_addr_cell("tmp" + std::to_string(this->data->memory_offset), this->data->memory_offset);
        symbol* cell_address = this->data->get_symbol("tmp" + std::to_string(this->data->memory_offset));
        this->code.push_back("STORE " + std::to_string(cell_address->offset));
        this->data->memory_offset++;
        this->pc += 4;

        return cell_address;
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
    if (sym->is_array_cell || sym->is_addr_cell) {
        return;
    }
    if (!sym->is_init) {
        if (sym->is_const) {
            this->init_const(sym);
        } else {
            throw std::string(sym->name + " - symbol is not initialized");
        }
    }
}

// ASSEMBLER COMMANDS

void Code::ADD(long long offset) {
    this->code.push_back("ADD " + std::to_string(offset));
}

void Code::SUB(long long offset) {
    this->code.push_back("SUB " + std::to_string(offset));
}

void Code::store(symbol* sym) {
    if (sym->is_addr_cell) {
        this->code.push_back("STOREI " + std::to_string(sym->offset));
    } else {
        this->code.push_back("STORE " + std::to_string(sym->offset));
    }
}

void Code::load(symbol* sym) {
    if (sym->is_addr_cell) {
        this->code.push_back("LOADI " + std::to_string(sym->offset));
    } else {
        this->code.push_back("LOAD " + std::to_string(sym->offset));
    }
}