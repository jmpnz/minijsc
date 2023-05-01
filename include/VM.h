#ifndef VM_H
#define VM_H

#include <utility>

#include "Bytecode.h"
#include "JSValue.h"

namespace minijsc {

/// Virtual machine context used during execution, context creates a value
/// pool to store constants and immediates.
struct VMContext {
    /// Store a value in the constants pool.
    auto storeConstant(JSBasicValue value) -> void {
        constantsPool.emplace_back(std::move(value));
    }

    /// Load a value from the constants pool.
    auto loadConstant(uint32_t offset) -> JSBasicValue {
        return constantsPool.at(offset);
    }

    private:
    /// Constants pool stores all constants created in the source code.
    std::vector<JSBasicValue> constantsPool;
};

/// Virtual machine class implements a stack based virtual machine.
class VM {
    /// Virtual machine interpretation results.
    enum class VMResult {
        Ok,
        CompileError,
        RuntimeError,
    };

    public:
    // VM constructor that we use to load bytecode for execution.
    explicit VM(Bytecode code)
        : code(std::move(code)), ctx(std::make_unique<VMContext>()) {}

    // Return next instruction to execute, incrementing the instruction pointer.
    inline auto fetch() -> uint32_t {
        auto nextIp = ip;
        ip++;
        return nextIp;
    }

    // Run the execution loop.
    auto run() -> void;

    private:
    // Instruction pointer, since we're not doing memory mapped I/O
    // and all execution is in a single context the instruction pointer
    // starts at the beginning of code.
    uint32_t ip = 0;
    // Bytecode to execute.
    Bytecode code;
    // Execution context.
    std::unique_ptr<VMContext> ctx;
};

} // namespace minijsc

#endif
