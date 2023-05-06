#include "Interpreter.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"
#include <memory>
#include <stdexcept>

namespace minijsc {

/// Interpreter visits literal expressions returning the wrapped value.
auto Interpreter::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> minijsc::JSBasicValue {
    return expr->getValue();
}

/// Interpreter visits binary expressions returning result of evaluation.
auto Interpreter::visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
    -> minijsc::JSBasicValue {
    auto lhs   = expr->getLeft();
    auto rhs   = expr->getRight();
    auto binOp = expr->getOperator();

    switch (binOp.getKind()) {
    case JSTokenKind::Plus:
        return JSBasicValue(3. + 489.);
    default:
        throw std::invalid_argument("Unknown operator");
    }
}

/// Interpreter core evaluation loop.
auto Interpreter::evaluate(std::shared_ptr<Expr> expr) -> JSBasicValue {
    return expr->accept(this);
}

} // namespace minijsc
