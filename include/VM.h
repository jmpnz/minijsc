#ifndef VM_H
#define VM_H

#include <memory>
#include <utility>

#include "Bytecode.h"
#include "JSValue.h"

#define DEBUG_TRACE_EXECUTION

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
    explicit VM(const Bytecode& bcode) {
        // code(std::move(code)), ctx(std::make_unique<VMContext>())
        code = bcode;
        ctx  = std::make_unique<VMContext>();
#ifdef DEBUG_TRACE_EXECUTION
        disas = Disassembler(bcode, "vm-trace");
#endif
    }

    // Return next instruction to execute, incrementing the instruction pointer.
    inline auto fetch() -> OPCode {
        auto nextIp = ip;
        ip++;
        return code.at(nextIp);
    }

    /// Store a value in the constants pool.
    auto storeConstant(JSBasicValue value) -> void {
        ctx->storeConstant(std::move(value));
    }

    /// Load a value from the constants pool.
    auto loadConstant(uint32_t offset) -> JSBasicValue {
        return ctx->loadConstant(offset);
    }

    // Run the execution loop.
    auto run() -> VMResult;

    private:
    // Instruction pointer, since we're not doing memory mapped I/O
    // and all execution is in a single context the instruction pointer
    // starts at the beginning of code.
    uint32_t ip = 0;
    // Bytecode to execute.
    Bytecode code;
    // Execution context.
    std::unique_ptr<VMContext> ctx;
#ifdef DEBUG_TRACE_EXECUTION
    // If debug flag is active load a disassembler
    Disassembler disas;
#endif
};

} // namespace minijsc

#endif
