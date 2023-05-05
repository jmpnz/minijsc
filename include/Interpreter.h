//===----------------------------------------------------------------------===//
// Interpreter.h: This file implements the baseline interpreter for minijsc
// the interpter like the bytecode compiler are implemented using a visitor
// patter.
//===----------------------------------------------------------------------===//
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"

namespace minijsc {

/// Interpreter implements runtime evaluation of the abstract syntax tree.
class Interpreter {
    /// Default constructor.
    explicit Interpreter() = default;

    // Interpret an AST node.
    auto interpret(JSExpr* expr) -> void;
};
} // namespace minijsc

#endif