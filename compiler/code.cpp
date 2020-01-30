#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "code.hpp"
#include "data.hpp"
#include "labels.hpp"
#include "symbol.hpp"

Code::Code(std::shared_ptr<Data> data) {
    this->pc = 0;
    this->data = data;
}

std::vector<std::string> Code::get_code() {
    return this->code;
}

void Code::end_code() {
    this->HALT();
}

// COMMANDS

void Code::assign(symbol* var) {
    if (var->is_iterator) {
        throw std::string("Cannot assign value to iterator - " + var->name);
    }
    this->store(var);
    var->is_init = true;
}

void Code::if_block(cond_label* label) {
    this->code[label->go_to] += std::to_string(this->pc);
}

cond_label* Code::if_else_first_block(cond_label* label) {
    this->JUMP();

    this->if_block(label);

    label->go_to = this->pc - 1;
    return label;
}

void Code::if_else_second_block(cond_label* label) {
    this->if_block(label);
}

void Code::while_block(cond_label* label) {
    this->JUMP(label->start);

    this->if_block(label);
}

cond_label* Code::do_while_first_block() {
    return new cond_label(0, this->pc - 1);
}

void Code::do_while_second_block(cond_label* label, cond_label* cond) {
    this->JUMP(label->go_to);

    this->if_block(cond);
}

for_label* Code::for_first_block(std::string iterator_name, symbol* start, symbol* end, bool to) {
    symbol* iterator = this->data->get_symbol(iterator_name);

    this->check_init(start);
    this->check_init(end);

    // unroll if known range is smaller than 10
    // if (start->is_const && end->is_const) {
    //     if (to) {
    //         if (end->value - start->value < 10 && end->value - start->value >= 0) {
    //             this->LOAD(start->offset);
    //             this->STORE(iterator->offset);
    //             cond_label* label = new cond_label(this->pc, 0);
    //             return new for_label(iterator, start, end, label, true);
    //         }
    //     } else {
    //         if (start->value - end->value < 10 && start->value - end->value >= 0) {
    //             this->LOAD(start->offset);
    //             this->STORE(iterator->offset);
    //             cond_label* label = new cond_label(this->pc, 0);
    //             return new for_label(iterator, start, end, label, true);
    //         }
    //     }
    // }

    // creating temporary variable for end and coping it
    std::string end_name = "END" + std::to_string(this->data->memory_offset);
    this->data->put_symbol(end_name, true);
    symbol* end_tmp = this->data->get_symbol(end_name);

    this->load(end);
    this->STORE(end_tmp->offset);

    // initializing iterator
    this->load(start);
    this->STORE(iterator->offset);

    cond_label* label = new cond_label(this->pc + 1, 0);
    this->SUB(end_tmp->offset);
    if (to) {
        this->JPOS();
    } else {
        this->JNEG();
    }

    return new for_label(iterator, start, end_tmp, label, false);
}

void Code::for_second_block(for_label* label, bool to) {
    if (label->unroll) {
        // this->LOAD(label->iterator->offset);
        // if (to) {
        //     this->INC();
        // } else {
        //     this->DEC();
        // }
        // this->STORE(label->iterator->offset);

        // long long range = llabs(label->end->value - label->start->value) + 1;
        // long long end = this->pc;
        
        // std::cout << range << std::endl;
        // std::vector<std::string>::iterator it;
        // std::vector<std::string>::iterator it_start = this->code.begin() + label->jump_label->start;
        // std::vector<std::string>::iterator it_end = this->code.begin() + end;

        // for (int i = 0; i < range; i++) {
        //     std::cout << i << std::endl;
        //     for (it = it_start; it != it_end; it++) {
        //         std::cout << *it << std::endl;
        //         this->code.push_back(*it);
        //     }
        // }
    } else {
        this->LOAD(label->iterator->offset);
        if (to) {
            this->INC();
        } else {
            this->DEC();
        }
        this->STORE(label->iterator->offset);
        this->JUMP(label->jump_label->start - 1);

        this->code[label->jump_label->start] += std::to_string(this->pc);
    }
}

void Code::write(symbol* sym) {
    this->check_init(sym);

    this->load(sym);
    this->PUT();
}

void Code::read(symbol* sym) {
    this->GET();
    this->store(sym);
    sym->is_init = true;
}

// EXPRESSIONS

void Code::load_value(symbol* sym) {
    this->check_init(sym);

    this->load(sym);
}

void Code::plus(symbol* a, symbol* b) {
    symbol* one = this->data->get_symbol("1");
    this->check_init(a);
    this->check_init(b);
    this->check_init(one);

    if (a == b) {
        this->load(a);
        this->SHIFT(one->offset);
        return;
    }

    if (b->is_const && llabs(b->value) < 10) {
        this->load(a);
        for (int i = 0; i < llabs(b->value); i++) {
            if (b->value > 0) {
                this->INC();
            } else {
                this->DEC();
            }
        }
        return;
    }

    if (a->is_addr_cell && b->is_addr_cell) {
        this->load(a);
        this->STORE(this->data->memory_offset);
        this->load(b);
        this->ADD(this->data->memory_offset);
    } else if (b->is_addr_cell && !a->is_addr_cell) {
        this->load(b);
        this->ADD(a->offset);
    } else {
        this->load(a);
        this->ADD(b->offset);
    }
}

void Code::minus(symbol* a, symbol* b) {
    this->check_init(a);
    this->check_init(b);

    if (a == b) {
        this->SUB(0);
        return;
    }

    if (b->is_const && llabs(b->value) < 10) {
        this->load(a);
        for (int i = 0; i < llabs(b->value); i++) {
            if (b->value < 0) {
                this->INC();
            } else {
                this->DEC();
            }
        }
        return;
    }

    if (b->is_addr_cell) {
        this->load(b);
        this->STORE(this->data->memory_offset);
        this->load(a);
        this->SUB(this->data->memory_offset);
    } else {
        this->load(a);
        this->SUB(b->offset);
    }
}

void Code::times(symbol* a, symbol* b) {
    symbol* A = this->data->get_symbol("A");
    symbol* B = this->data->get_symbol("B");
    symbol* C = this->data->get_symbol("C");
    symbol* one = this->data->get_symbol("1");
    symbol* minus_one = this->data->get_symbol("-1");

    this->check_init(one);
    this->check_init(a);
    this->check_init(b);

    if (b->is_const && b->value == 2) {
        this->load(a);
        this->SHIFT(one->offset);
        return;
    }

    this->check_init(minus_one);

    // result = 0 in register C
    this->SUB(0);
    this->STORE(C->offset);

    // copy a and b, and leave a in p0
    this->load(b);
    this->STORE(B->offset);
    this->load(a);
    this->STORE(A->offset);

    // flip a if its negative
    this->JPOS(this->pc + 4);
    this->SUB(A->offset);
    this->SUB(A->offset);
    this->STORE(A->offset);

    // while (a != 0) {
    this->JZERO(this->pc + 15);
    // if (a & 1) {
    this->SHIFT(minus_one->offset);
    this->SHIFT(one->offset);
    this->SUB(A->offset);

    this->JZERO(this->pc + 4);
    this->LOAD(C->offset);
    this->ADD(B->offset);
    this->STORE(C->offset);
    // }

    // b = b << 1
    this->LOAD(B->offset);
    this->SHIFT(one->offset);
    this->STORE(B->offset);
    // a = a >> 1
    this->LOAD(A->offset);
    this->SHIFT(minus_one->offset);
    this->STORE(A->offset);
    this->JUMP(this->pc - 14);
    // }

    // flip result if a was negative
    this->load(a);
    this->JPOS(this->pc + 4);
    this->SUB(0);
    this->SUB(C->offset);
    this->JUMP(this->pc + 2);

    this->LOAD(C->offset);
}

void Code::div(symbol* a, symbol* b) {
    symbol* A = this->data->get_symbol("A"); // dividend
    symbol* B = this->data->get_symbol("B"); // scaled_divisor
    symbol* C = this->data->get_symbol("C"); // result
    symbol* D = this->data->get_symbol("D"); // remain
    symbol* E = this->data->get_symbol("E"); // multiple
    symbol* one = this->data->get_symbol("1");
    symbol* minus_one = this->data->get_symbol("-1");

    this->check_init(minus_one);
    this->check_init(a);
    this->check_init(b);

    if (b->is_const && b->value == 2) {
        this->load(a);
        this->SHIFT(minus_one->offset);
        return;
    }

    this->check_init(one);

    if (a == b) {
        this->load(a);
        this->JZERO(this->pc + 3);
        this->LOAD(one->offset);
        return;
    }

    // check if b != 0
    this->load(b);
    this->JZERO(this->pc + 62); // Jump to end if b == 0
    // scaled_divisor = b
    this->STORE(B->offset);
    // flip b if its negative
    this->JPOS(this->pc + 4);
    this->SUB(B->offset);
    this->SUB(B->offset);
    this->STORE(B->offset);
    
    // result = 0
    this->SUB(0);
    this->STORE(C->offset);
    // multiple = 1 
    this->INC();
    this->STORE(E->offset);

    // remain = a, and a stays in p0
    this->load(a);
    this->STORE(A->offset);
    // flip a if its negative
    this->JPOS(this->pc + 4);
    this->SUB(A->offset);
    this->SUB(A->offset);
    this->STORE(A->offset);
    this->STORE(D->offset);

    // while (scaled_divisor < a) {
    this->SUB(B->offset);    
    this->JPOS(this->pc + 2);  
    this->JUMP(this->pc + 9);
    // scaled_divisor << 1
    this->LOAD(B->offset);
    this->SHIFT(one->offset);
    this->STORE(B->offset);
    // multiple << 1
    this->LOAD(E->offset);
    this->SHIFT(one->offset);
    this->STORE(E->offset);
    this->LOAD(A->offset);
    this->JUMP(this->pc - 10); 
    // }

    // do {
    // if(remain >= scaled_divisor) {
    this->LOAD(D->offset);
    this->SUB(B->offset);
    this->JNEG(this->pc + 5); 
    // remain -= scaled_divisor
    this->STORE(D->offset);
    // result += multiple
    this->LOAD(C->offset);
    this->ADD(E->offset);
    this->STORE(C->offset);
    // }
    // scaled_divisor >> 1
    this->LOAD(B->offset);
    this->SHIFT(minus_one->offset);
    this->STORE(B->offset);
    // multiple >> 1
    this->LOAD(E->offset);
    this->SHIFT(minus_one->offset);
    this->STORE(E->offset);
    // while (multiple != 0);
    this->JZERO(this->pc + 2);
    this->JUMP(this->pc - 14);

    this->load(a);
    this->JPOS(this->pc + 4);
    this->SUB(0);
    this->SUB(C->offset);
    this->STORE(C->offset);

    this->load(b);
    this->JPOS(this->pc + 4);
    this->SUB(0);
    this->SUB(C->offset);
    this->STORE(C->offset);

    // floor(result)
    this->LOAD(C->offset);
    this->JNEG(this->pc + 2);
    this->JUMP(this->pc + 7);
    this->LOAD(D->offset);
    this->JZERO(this->pc + 4);
    this->LOAD(C->offset);
    this->DEC();
    this->JUMP(this->pc + 2);
    this->LOAD(C->offset); 
}

void Code::mod(symbol* a, symbol* b) {
    symbol* A = this->data->get_symbol("A"); // dividend
    symbol* B = this->data->get_symbol("B"); // scaled_divisor
    symbol* C = this->data->get_symbol("C"); // result
    symbol* D = this->data->get_symbol("D"); // remain
    symbol* E = this->data->get_symbol("E"); // multiple
    symbol* one = this->data->get_symbol("1");
    symbol* minus_one = this->data->get_symbol("-1");

    this->check_init(one);
    this->check_init(minus_one);
    this->check_init(a);
    this->check_init(b);

    // a MOD 2
    if (b->is_const && b->value == 2) {
        this->load(a);
        this->STORE(A->offset);
        this->SHIFT(minus_one->offset);
        this->SHIFT(one->offset);
        this->SUB(A->offset);
        this->JZERO(this->pc + 3);
        this->SUB(0);
        this->INC();
        return;
    }

    if (a == b) {
        this->SUB(0);
        return;
    }

    // check if b != 0
    this->load(b);
    this->JZERO(this->pc + 59); // Jump to end if b == 0
    // scaled_divisor = b
    this->STORE(B->offset);
    // flip b if its negative
    this->JPOS(this->pc + 4);
    this->SUB(B->offset);
    this->SUB(B->offset);
    this->STORE(B->offset);
    
    // result = 0
    this->SUB(0);
    this->STORE(C->offset);
    // multiple = 1 
    this->INC();
    this->STORE(E->offset);

    // remain = a, and a stays in p0
    this->load(a);
    this->STORE(A->offset);
    // flip a if its negative
    this->JPOS(this->pc + 4);
    this->SUB(A->offset);
    this->SUB(A->offset);
    this->STORE(A->offset);
    this->STORE(D->offset);

    // while (scaled_divisor < a) {
    this->SUB(B->offset);    
    this->JPOS(this->pc + 2);
    this->JUMP(this->pc + 9);
    // scaled_divisor << 1
    this->LOAD(B->offset);
    this->SHIFT(one->offset);
    this->STORE(B->offset);
    // multiple << 1
    this->LOAD(E->offset);
    this->SHIFT(one->offset);
    this->STORE(E->offset);
    this->LOAD(A->offset);
    this->JUMP(this->pc - 10);
    // }

    // do {
    // if(remain >= scaled_divisor) {
    this->LOAD(D->offset);
    this->SUB(B->offset);
    this->JNEG(this->pc + 5);
    // remain -= scaled_divisor
    this->STORE(D->offset);
    // result += multiple
    this->LOAD(C->offset);
    this->ADD(E->offset);
    this->STORE(C->offset);
    // }
    // scaled_divisor >> 1
    this->LOAD(B->offset);
    this->SHIFT(minus_one->offset);
    this->STORE(B->offset);
    // multiple >> 1
    this->LOAD(E->offset);
    this->SHIFT(minus_one->offset);
    this->STORE(E->offset);
    // while (multiple != 0);
    this->JZERO(this->pc + 2);
    this->JUMP(this->pc - 14);

    // modulo fiku miku
    
    // if a mod b = 0 return 0
    this->LOAD(D->offset);
    this->JZERO(this->pc + 15);

    // testing signs
    this->load(a);
    this->JPOS(this->pc + 8);
    // a is negative
    this->load(b);
    this->JPOS(this->pc + 4);
    // a < 0 && b < 0  -> a mod b = - (a % b)
    this->SUB(0);
    this->SUB(D->offset);
    this->JUMP(this->pc + 8); // to end
    // a < 0 && b > 0  -> a mod b = b - (a % b)
    this->SUB(D->offset);
    this->JUMP(this->pc + 6); // to end
    // a is positive
    this->load(b);
    this->JPOS(this->pc + 3);
    // a > 0 && b < 0  -> a mod b = b + (a % b)
    this->ADD(D->offset);
    this->JUMP(this->pc + 2); // to end
    // a > 0 && b > 0  -> a mod b = a % b
    this->LOAD(D->offset);
}

// CONDITIONS

cond_label* Code::eq(symbol* a, symbol* b) {
    long long start = this->pc;
    
    if (b)

    this->minus(a, b);
    this->JZERO(this->pc + 2);
    this->JUMP();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
}

cond_label* Code::neq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JZERO();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
}

cond_label* Code::le(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JNEG(this->pc + 2);
    this->JUMP();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
}

cond_label* Code::ge(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JPOS(this->pc + 2);
    this->JUMP();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
}

cond_label* Code::leq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JPOS();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
}

cond_label* Code::geq(symbol* a, symbol* b) {
    long long start = this->pc;
    this->minus(a, b);
    this->JNEG();
    long long addr = this->pc - 1;

    cond_label* label = new cond_label(start, addr);
    return label;
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
        if (!sym->is_array) {
            return sym;
        } else {
            throw std::string(name + " - wrong usage, " + name + " is an array");
        }
    } else {
        throw std::string(name + " - symbol does not exist");
    }
}

symbol* Code::array_num_pidentifier(std::string name, long long num) {
    symbol* sym = this->data->get_symbol(name);
    if (sym != nullptr) {
        if (sym->is_array) {
            if (sym->array_start <= num && sym->array_end >= num) {
                long long offset = num - sym->array_start + sym->offset;

                if (!this->data->check_context(name + std::to_string(num))) {
                    this->data->put_array_cell(name + std::to_string(num), offset);
                }

                symbol* ret_sym = this->data->get_symbol(name + std::to_string(num));
                return ret_sym;
            } else {
                throw std::string(name + " - index out of boundry");
            }
        } else {
            throw std::string(name + " - is not an array");
        }
    } else {
        throw std::string(name + " - array does not exist");
    }
}

symbol* Code::array_pid_pidentifier(std::string name, std::string pid_name) {
    symbol* array = this->data->get_symbol(name);
    symbol* var = this->data->get_symbol(pid_name);

    if (array != nullptr && var != nullptr) {
        if (array->is_array) {
            // init constants
            symbol* array_start = this->get_num(array->array_start);
            this->check_init(array_start);
            symbol* array_offset = this->get_num(array->offset);
            this->check_init(array_offset);

            // calculate address of a cell
            this->LOAD(var->offset);
            this->SUB(array_start->offset);
            this->ADD(array_offset->offset);

            // save address
            this->data->put_addr_cell("TMP" + std::to_string(this->data->memory_offset), this->data->memory_offset);
            symbol* cell_address = this->data->get_symbol("TMP" + std::to_string(this->data->memory_offset));
            this->STORE(cell_address->offset);
            this->data->memory_offset++;

            return cell_address;
        } else {
            throw std::string(array->name + " - is not an array");
        }
    } else {
        if (array == nullptr) {
            throw std::string(name + " - array does not exsit");
        } else {
            throw std::string(pid_name + " - is not declared");
        }
    }
}

// MISC

void Code::generate_constant(long long value, long long offset) {
    long long digits[64];
    bool nonnegative = (value >= 0);

    symbol* one = this->data->get_symbol("1");
    if (!one->is_init) {
        this->SUB(0);
        this->INC();
        this->STORE(one->offset);
        one->is_init = true;
        one->value = 1;
    }

    this->SUB(0);

    if (llabs(value) < 10) {
        if (value > 0) {
            for (long long i = 0; i < value; i++) {
                this->INC();
            }
        } else {
            for (long long i = 0; i < -value; i++) {
               this->DEC();
            }
        }
    } else {
        long long i = 0;
        while (value != 0) {
            digits[i] = llabs(value % 2);
            value /= 2;
            i++;
        }
        i--;

        for (; i > 0; i--) {
            if (digits[i] == 1) {
                if (nonnegative) {
                    this->INC();
                } else {
                    this->DEC();
                }
            }
            this->SHIFT(one->offset);
        }

        if (digits[i] == 1) {
            if (nonnegative) {
                this->INC();
            } else {
                this->DEC();
            }
        }
    }

    this->STORE(offset);
}

void Code::init_const(symbol* sym) {
    this->data->init_constant(sym->name, std::stoll(sym->name));
    this->generate_constant(std::stoll(sym->name), sym->offset);
}

void Code::check_init(symbol* sym) {
    if (sym->is_array_cell || sym->is_addr_cell || sym->is_iterator) {
        return;
    }
    if (!sym->is_init) {
        if (sym->is_const) {
            this->init_const(sym);
        } else {
            std::cout << sym->name << " - symbol may be uninitialized" << std::endl;
        }
    }
}

// ASSEMBLER COMMANDS

void Code::HALT() {
    this->code.push_back("HALT");
    this->pc++;
}

void Code::GET() {
    this->code.push_back("GET");
    this->pc++;
}

void Code::PUT() {
    this->code.push_back("PUT");
    this->pc++;
}

void Code::ADD(long long offset) {
    this->code.push_back("ADD " + std::to_string(offset));
    this->pc++;
}

void Code::SUB(long long offset) {
    this->code.push_back("SUB " + std::to_string(offset));
    this->pc++;
}

void Code::SHIFT(long long offset) {
    this->code.push_back("SHIFT " + std::to_string(offset));
    this->pc++;
}

void Code::INC() {
    this->code.push_back("INC");
    this->pc++;
}

void Code::DEC() {
    this->code.push_back("DEC");
    this->pc++;
}

void Code::STORE(long long offset) {
    this->code.push_back("STORE " + std::to_string(offset));
    this->pc++;
}

void Code::LOAD(long long offset) {
    this->code.push_back("LOAD " + std::to_string(offset));
    this->pc++;
}

void Code::store(symbol* sym) {
    if (sym->is_addr_cell) {
        this->code.push_back("STOREI " + std::to_string(sym->offset));
    } else {
        this->code.push_back("STORE " + std::to_string(sym->offset));
    }
    this->pc++;
}

void Code::load(symbol* sym) {
    if (sym->is_addr_cell) {
        this->code.push_back("LOADI " + std::to_string(sym->offset));
    } else {
        this->code.push_back("LOAD " + std::to_string(sym->offset));
    }
    this->pc++;
}

void Code::JUMP() {
    this->code.push_back("JUMP ");
    this->pc++;
}

void Code::JUMP(long long offset) {
    this->code.push_back("JUMP " + std::to_string(offset));
    this->pc++;
}

void Code::JPOS() {
    this->code.push_back("JPOS ");
    this->pc++;
}

void Code::JPOS(long long offset) {
    this->code.push_back("JPOS " + std::to_string(offset));
    this->pc++;
}

void Code::JNEG() {
    this->code.push_back("JNEG ");
    this->pc++;
}

void Code::JNEG(long long offset) {
    this->code.push_back("JNEG " + std::to_string(offset));
    this->pc++;
}

void Code::JZERO() {
    this->code.push_back("JZERO ");
    this->pc++;
}

void Code::JZERO(long long offset) {
    this->code.push_back("JZERO " + std::to_string(offset));
    this->pc++;
}
