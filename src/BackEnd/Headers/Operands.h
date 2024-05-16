#ifndef COMPILER_OPERAND_H
#define COMPILER_OPERAND_H

#include <string>
#include <memory>

namespace T86 {

    /**
     * Operand of the target code
     */
    class Operand {
    public:
        virtual std::string toString() = 0;

        virtual std::unique_ptr<Operand> clone() const = 0;

    private:
    };

    class IntImmediate : public Operand {
    public:
        IntImmediate(long long = 0);

        void addValue(long long);

        std::string toString() override;

        std::unique_ptr<Operand> clone() const override;

    private:
        long long value;

    };

    class DoubleImmediate : public Operand {
    public:
        DoubleImmediate(double = 0);

        std::string toString() override;

        std::unique_ptr<Operand> clone() const override;

    private:
        double value;

    };

    class Register : public Operand {
    public:
        Register(std::size_t = 0, long long = 0);

        enum SecialRegisters {
            SP = ULLONG_MAX - 1,
            BP = ULLONG_MAX - 2,
            IP = ULLONG_MAX - 3,
        };

        std::string toString() override;

        std::unique_ptr<Operand> clone() const override;

    private:
        std::size_t register_number;

        long long offset = 0;
    };

    class FRegister : public Operand {
    public:
        FRegister(std::size_t = 0);

        std::string toString() override;

        std::unique_ptr<Operand> clone() const override;

    private:
        std::size_t reg_number;

    };

    class Memory : public Operand {
    public:
        Memory(std::unique_ptr<Operand> &&);

        Memory(const Memory&);

        std::string toString() override;

        std::unique_ptr<Operand> clone() const override;

    private:
        std::unique_ptr<Operand> addr;

    };
}

#endif //COMPILER_OPERAND_H
