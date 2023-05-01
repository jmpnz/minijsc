#include "Bytecode.h"

#include <cstdint>
#include <cstdio>

#include <string>

namespace minijsc {

auto Disassembler::disassemble() -> void {
    printf("== %s ==\n", name.c_str());

    for (size_t offset = 0; offset < code.size();) {
        offset = disassembleInstruction(offset);
    }
}

auto Disassembler::disassembleInstruction(size_t offset) -> size_t {
    // Print the instruction offset.
    printf("%04zu", offset);
    // Grab the instruction's from the offset.
    auto instruction = code.at(offset);

    // Helpful lambdas to print things.
    auto instPrinter = [&offset](const std::string& name) {
        printf("    %s\n", name.c_str());
        return offset + 1;
    };

    switch (instruction) {
    case OPCode::OPReturn:
        return instPrinter("OP_RETURN");
    default:
        printf("Unknown instruction %hhu at offset %zu", instruction, offset);
        return offset + 1;
    }
}

} // namespace minijsc
