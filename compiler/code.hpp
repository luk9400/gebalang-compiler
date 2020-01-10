#include <string>
#include <vector>
#include <memory>

#include "data.hpp"
#include "labels.hpp"

class Code {
    private:
        // program counter
        long long pc;
        std::vector<std::string> code;
        std::shared_ptr<Data> data;

    public:
        Code(std::shared_ptr<Data> data);

        std::vector<std::string> get_code();
        void end_code();

        // COMMANDS

        void assign(symbol* var);
        void if_block(cond_label* label);
        void write(symbol* sym);
        void read(symbol* sym);

        // EXPRESSIONS

        void load_value(symbol* sym);
        void plus(symbol* a, symbol* b);
        void minus(symbol* a, symbol* b);

        // CONDITIONS
        cond_label* eq(symbol* a, symbol* b);

        // VALUES & PIDs

        symbol* get_num(long long num);
        symbol* pidentifier(std::string name);
        symbol* array_num_pidentifier(std::string name , long long num);
        symbol* array_pid_pidentifier(std::string name, std::string pid_name);

        // MISC

        void generate_constant(long long value, long long offset);
        void init_const(symbol* sym);

        /**
         *  Checks if symbol is initialized,
         *  if not and symbol is constant, it initializes it,
         *  otherwise throws exception 
         */
        void check_init(symbol* sym);

        // ASSEMBLER COMMANDS
        void ADD(long long offset);
        void SUB(long long offset);
        void store(symbol* sym);
        void load(symbol* sym);
};
