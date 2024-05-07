#include "Operands.h"


T86::IntImmediate::IntImmediate(long long new_value) : value(new_value) {}

void T86::IntImmediate::addValue(long long new_value) {
    value = new_value;
}

std::string T86::IntImmediate::toString() {
    return std::to_string(value);
}

std::unique_ptr<T86::Operand> T86::IntImmediate::clone() const {
    return std::make_unique<T86::IntImmediate>(*this);
}

T86::DoubleImmediate::DoubleImmediate(double new_value) : value(new_value) {}

std::string T86::DoubleImmediate::toString() {
    return std::to_string(value);
}

std::unique_ptr<T86::Operand> T86::DoubleImmediate::clone() const {
    return std::make_unique<T86::DoubleImmediate>(*this);
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

std::unique_ptr<T86::Operand> T86::Register::clone() const {
    return std::make_unique<T86::Register>(*this);
}

T86::FRegister::FRegister(std::size_t new_reg) : reg_number(new_reg) {}

std::string T86::FRegister::toString() {
    return "F" + std::to_string(reg_number);
}

std::unique_ptr<T86::Operand> T86::FRegister::clone() const {
    return std::make_unique<T86::FRegister>(*this);
}

T86::Memory::Memory(std::unique_ptr<Operand> &&new_addr) : addr(std::move(new_addr)) {}

std::string T86::Memory::toString() {
    return '[' + addr->toString() + ']';
}

T86::Memory::Memory(const Memory&old_mem){
    addr = old_mem.addr->clone();
}

std::unique_ptr<T86::Operand> T86::Memory::clone() const {
    return std::make_unique<T86::Memory>(*this);
}