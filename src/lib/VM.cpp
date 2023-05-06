#include "VM.h"
#include "Bytecode.h"

#include "JSValue.h"
#include "fmt/core.h"

#include <string>

namespace minijsc {

/// Run the virtual machine, executing the bytecode loaded.
auto VM::run() -> VMResult {
    while (ip <= code.size()) {
        auto inst = fetch();
#ifdef DEBUG_TRACE_EXECUTION
        // Since ip is incremented in fetch() we need to substract
        // 1 to grab the proper offset.
        // TODO: maybe switch disassembleInstruction to take an OPCode
        auto prev = ip - 1;
        disas.disassembleInstruction(prev);
#endif
        switch (inst) {
        case OPCode::Return: {
            return VMResult::Ok;
        }
        case OPCode::Constant: {
            // Next opcode after OPConstant is the byte offset
            // in the constants pool.
            auto offset = fetch();
            //   fmt::print("Next OPCode : {}\n", (int)offset);
            JSBasicValue const value = ctx->loadConstant((size_t)offset);
            //  fmt::print(stdout, "JSBasicValue: {}\n", value.toString());
            push(value);
            break;
        }
        case OPCode::Negate: {
            // Pop value from the stack and negate it.
            JSBasicValue const value = pop();
            push(JSBasicValue(-value.getValue<JSNumber>()));
            break;
        }
        case OPCode::Add: {
            JSBasicValue const rhs = pop();
            JSBasicValue const lhs = pop();
            JSBasicValue const sum =
                lhs.getValue<JSNumber>() + rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Sub: {
            JSBasicValue const rhs = pop();
            JSBasicValue const lhs = pop();
            JSBasicValue const sum =
                lhs.getValue<JSNumber>() - rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Mul: {
            JSBasicValue const rhs = pop();
            JSBasicValue const lhs = pop();
            JSBasicValue const sum =
                lhs.getValue<JSNumber>() * rhs.getValue<JSNumber>();
            push(sum);
            break;
        }

        case OPCode::Div: {
            JSBasicValue const rhs = pop();
            JSBasicValue const lhs = pop();
            JSBasicValue const sum =
                lhs.getValue<JSNumber>() / rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        default:
            fmt::print("Unexpected instruction {} ", (uint8_t)inst);
        }
    }

    return VMResult::Ok;
}

/// Display the contents of stack.
auto VM::displayStack() -> void {
    fmt::print("        ");
    for (const auto& value : stack) {
        fmt::print("[ {} ]", value.toString());
    }
    fmt::print("\n");
}

} // namespace minijsc
