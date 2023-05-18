#include "Bytecode.h"
#include "VM.h"

#include "JSValue.h"
#include "fmt/core.h"

#include <cassert>
#include <string>

namespace minijsc {

/// Run the virtual machine, executing the bytecode loaded.
auto VM::run() -> VMResult {
    while (ip < code.size()) {
        auto inst = fetch();
        // Since ip is incremented in fetch() we need to substract
        // 1 to grab the proper offset.
        // TODO: maybe switch disassembleInstruction to take an OPCode
        // auto prev = ip - 1;
        // disas.disassembleInstruction(prev);
        // fmt::print("Executing instruction : {}\n", (uint8_t)inst);
        switch (inst) {
        case OPCode::Return: {
            return VMResult::Ok;
        }
        case OPCode::Constant: {
            // Next opcode after OPConstant is the byte offset
            // in the constants pool.
            auto offset = fetch();
            assert(ctx != nullptr);
            JSBasicValue value = ctx->loadConstant((size_t)offset);
            push(value);
            break;
        }
        case OPCode::Negate: {
            // Pop value from the stack and negate it.
            JSBasicValue value = pop();
            push(JSBasicValue(-value.getValue<JSNumber>()));
            break;
        }
        case OPCode::Not: {
            JSBasicValue value = pop();
            push(JSBasicValue(JSBasicValue(!isTruthy(value))));
            break;
        }
        case OPCode::And: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            bool isTrue      = isTruthy(lhs) && isTruthy(rhs);
            push(JSBasicValue(isTrue));
            break;
        }
        case OPCode::Or: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            bool isTrue      = isTruthy(lhs) || isTruthy(rhs);
            push(JSBasicValue(isTrue));
            break;
        }
        case OPCode::Add: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue sum =
                lhs.getValue<JSNumber>() + rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Sub: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue sum =
                lhs.getValue<JSNumber>() - rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Mul: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue sum =
                lhs.getValue<JSNumber>() * rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Div: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue sum =
                lhs.getValue<JSNumber>() / rhs.getValue<JSNumber>();
            push(sum);
            break;
        }
        case OPCode::Equal: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue res = lhs.isEqual(rhs);
            push(res);
            break;
        }
        case OPCode::NotEqual: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue res = !lhs.isEqual(rhs);
            push(res);
            break;
        }
        case OPCode::GreaterEqual:
        case OPCode::Greater: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue res = lhs.greaterOrEqual(rhs);
            push(res);
            break;
        }
        case OPCode::LesserEqual:
        case OPCode::Lesser: {
            JSBasicValue rhs = pop();
            JSBasicValue lhs = pop();
            JSBasicValue res = lhs.lesserOrEqual(rhs);
            push(res);
            break;
        }
        default:
            fmt::print("Unexpected instruction {} ", (uint8_t)inst);
            break;
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
