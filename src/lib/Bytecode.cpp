#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/format.h"

#include "Bytecode.h"

#include <cstdint>
#include <cstdio>

#include <string>

namespace minijsc {

auto Disassembler::disassemble() -> void {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "==== {} ====\n",
               name);
    for (size_t offset = 0; offset < code.size();) {
        offset = disassembleInstruction(offset);
    }
}

auto Disassembler::disassembleInstruction(size_t offset) -> size_t {
    // Print the instruction offset.
    fmt::print("{:04}", offset);
    // Grab the instruction's from the offset.
    auto instruction = code.at(offset);

    // Helpful lambdas to print things.
    auto instPrinter = [&offset](const std::string& name) {
        fmt::print("        {}\n", name);
        return offset + 1;
    };

    switch (instruction) {
    case OPCode::Constant:
        return instPrinter("OP_CONSTANT");
    case OPCode::Add:
        return instPrinter("OP_ADD");
    case OPCode::Sub:
        return instPrinter("OP_SUB");
    case OPCode::Mul:
        return instPrinter("OP_MUL");
    case OPCode::Div:
        return instPrinter("OP_DIV");
    case OPCode::Negate:
        return instPrinter("OP_NEGATE");
    case OPCode::Return:
        return instPrinter("OP_RETURN");
    default:
        fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                   "\nUnknown instruction {:04} at offset {:04}\n",
                   (uint8_t)instruction, offset);
        return offset + 1;
    }
}

#ifdef EASTER_EGG

#define QUOTE "Omnia mea mecum porto"

#endif

} // namespace minijsc
