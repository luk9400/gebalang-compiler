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

        void write();

        void constant(long long value);
};
