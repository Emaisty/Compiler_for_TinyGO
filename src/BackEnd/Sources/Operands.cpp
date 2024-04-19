#include "Operands.h"


T86::IntImmediate::IntImmediate(long long new_value) : value(new_value) {}

std::string T86::IntImmediate::toString() {
    return std::to_string(value);
}

T86::DoubleImmediate::DoubleImmediate(double new_value) : value(new_value) {}

std::string T86::DoubleImmediate::toString() {
    return std::to_string(value);
}


T86::Register::Register(std::size_t new_reg, long long new_offset) : register_number(new_reg), offset(new_offset) {}

std::string T86::Register::toString() {
    std::string res;
    if (register_number == BP)
        res = "BP";
    else if (register_number == SP)
        res = "SP";
    else if (register_number == IP)
        res = "IP";
    else
        res = 'R' + std::to_string(register_number);

    if (offset != 0)
        res += " + " + std::to_string(offset);
    return res;
}

T86::Memory::Memory(std::unique_ptr<Operand> &&new_addr) : addr(std::move(new_addr)) {}

std::string T86::Memory::toString() {
    return '[' + addr->toString() + ']';
}