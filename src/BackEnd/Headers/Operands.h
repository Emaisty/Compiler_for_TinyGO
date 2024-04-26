#ifndef COMPILER_OPERAND_H
#define COMPILER_OPERAND_H

#include <string>
#include <memory>

namespace T86 {
    class Operand {
    public:
        virtual std::string toString() = 0;

    private:
    };

    class IntImmediate : public Operand {
    public:
        IntImmediate(long long = 0);

        void addValue(long long);

        std::string toString() override;

    private:
        long long value;

    };

    class DoubleImmediate : public Operand {
    public:
        DoubleImmediate(double = 0);

        std::string toString() override;

    private:
        double value;

    };

    class Register : public Operand {
    public:
        Register(std::size_t = 0, long long = 0);

        enum SecialRegisters {
            SP = LLONG_MAX - 1,
            BP = LLONG_MAX - 2,
            IP = LLONG_MAX - 3,
        };

        std::string toString() override;

    private:
        std::size_t register_number;

        long long offset = 0;
    };

    class Memory : public Operand {
    public:
        Memory(std::unique_ptr<Operand> &&);

        std::string toString() override;

    private:
        std::unique_ptr<Operand> addr;

    };
}

#endif //COMPILER_OPERAND_H
