#include <string>
#include <vector>
#include <memory>

#include "symbol.hpp"
#include "data.hpp"
#include "code.hpp"

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

void Code::write(long long offset) {
    this->code.push_back("LOAD " + std::to_string(offset));
    this->code.push_back("PUT");
    this->pc += 2;
}

void Code::read(long long offset) {
    this->code.push_back("GET");
    this->code.push_back("STORE " + std::to_string(offset));
    this->pc += 2;
}

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

long long Code::get_num(long long num) {
    // get constant if its initialized
    if (this->data->check_context(std::to_string(num))) {
        return this->data->get_symbol(std::to_string(num))->offset;
    } else { // initialize constant and put it in p0
        this->data->put_symbol(std::to_string(num));
        this->data->init_constant(std::to_string(num), num);

        long long offset = this->data->get_symbol(std::to_string(num))->offset;

        this->generate_constant(num, offset);

        return offset;
    }
}

void Code::assign(long long var_offset, long long value_offset) {
        this->code.push_back("LOAD " + std::to_string(value_offset));
        this->code.push_back("STORE " + std::to_string(var_offset));
        this->pc += 2;
}

long long Code::pidentifier(std::string name) {
    symbol* sym = this->data->get_symbol(name);
    if (sym != nullptr) {
        return sym->offset;
    }
}
