//===----------------------------------------------------------------------===//
// VM.h: This header defines the class and utilities used in the core virtual
// machine for minijsc.
//===----------------------------------------------------------------------===//
#ifndef VM_H
#define VM_H

#include <memory>
#include <utility>
#include <vector>

#include "Bytecode.h"
#include "JSValue.h"

#define DEBUG_TRACE_EXECUTION

namespace minijsc {

/// Stack can support up to 2**16 values.
static constexpr size_t kMaxStackSize = 65536;

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

/// Virtual machine stack.
using VMStack = std::vector<JSBasicValue>;

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
        code = bcode;
        ctx  = std::make_shared<VMContext>();
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
    auto storeConstant(const JSBasicValue& value) -> void {
        ctx->storeConstant(value);
    }

    /// Load a value from the constants pool.
    auto loadConstant(uint32_t offset) -> JSBasicValue {
        return ctx->loadConstant(offset);
    }

    // Run the execution loop.
    auto run() -> VMResult;

    // Push value onto the stack.
    auto push(const JSBasicValue& value) -> void { stack.push_back(value); }

    // Pop value from the stack.
    auto pop() -> JSBasicValue {
        auto value = stack.back();
        stack.pop_back();
        return value;
    }

    // Display the stack contents.
    auto displayStack() -> void;

    private:
    // Instruction pointer, since we're not doing memory mapped I/O
    // and all execution is in a single context the instruction pointer
    // starts at the beginning of code.
    uint32_t ip = 0;
    // Bytecode to execute.
    Bytecode code;
    // Virtual machine's stack.
    VMStack stack;
    // Execution context.
    std::shared_ptr<VMContext> ctx;
#ifdef DEBUG_TRACE_EXECUTION
    // If debug flag is active load a disassembler
    Disassembler disas;
#endif
};

} // namespace minijsc

#endif
