//===----------------------------------------------------------------------===//
// Interpreter.h: This header defines the class and interface for the baseline
// interpreter for minijsc. The interpreter is a REPL based interpreter with
// an evaluate loop. Evaluation is done at the abstract syntax tree level.
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
