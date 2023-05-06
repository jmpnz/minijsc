//===----------------------------------------------------------------------===//
// Interpreter.h: This header defines the class and interface for the baseline
// interpreter for minijsc. The interpreter is a REPL based interpreter with
// an evaluate loop. Evaluation is done at the abstract syntax tree level.
//===----------------------------------------------------------------------===//
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "AST.h"
#include "JSValue.h"
#include <memory>

namespace minijsc {

/// Interpreter implements runtime evaluation of the abstract syntax tree.
class Interpreter : public Visitor {
    public:
    /// Default constructor.
    explicit Interpreter()  = default;
    ~Interpreter() override = default;

    auto evaluate(std::shared_ptr<Expr> expr) -> JSBasicValue;

    auto visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
        -> JSBasicValue override;
    auto visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
        -> JSBasicValue override;
};
} // namespace minijsc

#endif
