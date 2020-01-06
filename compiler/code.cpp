#include <string>
#include <vector>
#include <memory>

#include "data.hpp"
#include "code.hpp"

Code::Code(std::shared_ptr<Data> data) {
    this->pc = 0;
    this->data = data;
}

void Code::end_code() {
    this->code.push_back("HALT");
    this->pc++;
}

std::vector<std::string> Code::get_code() {
    return this->code;
}

void Code::write() {
    this->code.push_back("LOAD 0");
    this->code.push_back("PUT");
    this->pc++;
}

void Code::constant(long long value) {
    this->code.push_back("SUB 0");
    if (value > 0) {
        for (long long i = 0; i < value; i++) {
            this->code.push_back("INC");
        }
    }
    if (value < 0) {
        for (long long i = 0; i < value; i++) {
            this->code.push_back("DEC");
        }
    }
    
    this->code.push_back("STORE" + this->data->memory_offset);
    this->data->memory_offset++;
}
