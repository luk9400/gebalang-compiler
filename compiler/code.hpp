#include <string>
#include <vector>
#include <memory>

#include "data.hpp"

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

        void write(long long offset);
        void read(long long offset);

        long long get_num(long long num);

        void generate_constant(long long value, long long offset);

        void assign(long long var_offset, long long value_offset);

        long long pidentifier(std::string name);
};
