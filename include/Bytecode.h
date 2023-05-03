#ifndef BYTECODE_H
#define BYTECODE_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace minijsc {

/// The bytecode virtual machine executes fixed size instructions
/// each instruction is one byte long, with the semantic context
/// encoded in the instruction itself.
/// The bytecode virtual machine is stack based, instructions such
/// loading or storing immediates are backed by a higher level object
/// pool.
enum class OPCode : uint8_t {
    Return,
    Constant,
    Negate,
    Add,
    Sub,
    Mul,
    Div,
};

/// Bytecode is a sequence of opcodes.
using Bytecode = std::vector<OPCode>;

/// @brief Disassembler consumes bytecodes and prints individual instructions with
/// their offsets to stdout.
/// The disassembler is mainly used to help with debugging the virtual machine.
class Disassembler {
    public:
    explicit Disassembler() = default;

    /// Constructor takes the bytecode to disassemble and a program
    // name to debug.
    explicit Disassembler(Bytecode code, std::string name)
        : code(std::move(code)), name(std::move(name)) {}

    /// Disassemble the loaded bytecode.
    auto disassemble() -> void;
    /// Disassemble the instruction at the given offset.
    auto disassembleInstruction(size_t offset) -> size_t;

    private:
    /// Bytecode loaded into the disassembler.
    Bytecode code;
    /// Name of the program to debug.
    std::string name;
};

} // namespace minijsc

#endif
