#include "Interpreter.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"
#include "Runtime.h"
#include "fmt/core.h"
#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace minijsc {

/// Interpreter visits expression statements evaluating the expression
/// and returning.
auto Interpreter::visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void {
    evaluate(stmt->getExpr().get());
}

/// Interpreter visits a block statement executing the statements within
/// the block.
auto Interpreter::visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void {
    executeBlock(block.get(), std::make_unique<Environment>());
}

/// Interpreter visits variable declarations defining bindings in the runtime
/// environment.
auto Interpreter::visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void {
    auto value = JSBasicValue();
    if (stmt->getInitializer() != nullptr) {
        value = evaluate(stmt->getInitializer().get());
    }
    assert(env != nullptr);
    env->defineBinding(stmt->getName(), value);
}

/// Interpreter visits variable expressions returning the value.
auto Interpreter::visitVarExpr(std::shared_ptr<JSVarExpr> expr)
    -> JSBasicValue {
    return env->resolveBinding(expr->getName());
}

/// Interpreter visits assignment expression, evaluating the right handside
/// and assigning it to the left handside.
auto Interpreter::visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
    -> JSBasicValue {
    auto value = evaluate(expr->getValue().get());
    env->defineBinding(expr->getName().getLexeme(), value);
    return value;
}

/// Interpreter visits literal expressions returning the wrapped value.
auto Interpreter::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> JSBasicValue {
    return expr->getValue();
}

/// Interpreter visits binary expressions returning result of evaluation.
auto Interpreter::visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
    -> JSBasicValue {
    auto lhs   = evaluate(expr->getLeft().get());
    auto rhs   = evaluate(expr->getRight().get());
    auto binOp = expr->getOperator();

    switch (binOp.getKind()) {
    case JSTokenKind::Plus:
        // TODO: handle upcasting of string/number lhs or rhs.
        return {lhs.getValue<JSNumber>() + rhs.getValue<JSNumber>()};
    case JSTokenKind::Minus:
        return {lhs.getValue<JSNumber>() - rhs.getValue<JSNumber>()};
    case JSTokenKind::Star:
        return {lhs.getValue<JSNumber>() * rhs.getValue<JSNumber>()};
    case JSTokenKind::Slash:
        return {lhs.getValue<JSNumber>() / rhs.getValue<JSNumber>()};
    // Comparison operations.
    case JSTokenKind::Greater:
        return JSBoolean(lhs.getValue<JSNumber>() > rhs.getValue<JSNumber>());
    case JSTokenKind::GreaterEqual:
        return JSBoolean(lhs.getValue<JSNumber>() >= rhs.getValue<JSNumber>());
    case JSTokenKind::Less:
        return JSBoolean(lhs.getValue<JSNumber>() < rhs.getValue<JSNumber>());
    case JSTokenKind::LessEqual:
        return JSBoolean(lhs.getValue<JSNumber>() <= rhs.getValue<JSNumber>());
    case JSTokenKind::BangEqual:
        return JSBoolean(lhs.getValue<JSNumber>() != rhs.getValue<JSNumber>());
    case JSTokenKind::EqualEqual:
        return JSBoolean(lhs.getValue<JSNumber>() == rhs.getValue<JSNumber>());
    default:
        throw std::invalid_argument("Unknown operator");
    }
}

/// Interpreter visits unary expressions returning result of evaluation.
auto Interpreter::visitUnaryExpr(std::shared_ptr<JSUnaryExpr> expr)
    -> JSBasicValue {
    auto rhs     = evaluate(expr->getRight().get());
    auto unaryOp = expr->getOperator();
    switch (unaryOp.getKind()) {
    case JSTokenKind::Minus:
        return {-rhs.getValue<JSNumber>()};
    case JSTokenKind::Bang:
        fmt::print("Unary/!/");
        return {!isTruthy(rhs)};
    default:
        fmt::print("visitUnaryExpr::nullPtr");
        return {nullptr};
    }
}

/// Interpreter visits grouping expression evaluating the grouped expression.
auto Interpreter::visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
    -> JSBasicValue {
    return evaluate(expr->getExpr().get());
}

/// Run the interpreter evaluation loop.
auto Interpreter::run(const std::vector<std::shared_ptr<JSStmt>>& stmts)
    -> void {
    try {
        for (const auto& stmt : stmts) {
            execute(stmt.get());
        }
    } catch (const std::runtime_error& e) {
        fmt::print("Runtime error : {}", e.what());
        return;
    }
}

/// Evaluate expressions.
auto Interpreter::evaluate(JSExpr* expr) -> JSBasicValue {
    return expr->accept(this);
}

/// Execute a single statement.
auto Interpreter::execute(JSStmt* stmt) -> void { return stmt->accept(this); }

/// Execute a block.
auto Interpreter::executeBlock(JSBlockStmt* block,
                               std::unique_ptr<Environment> env) -> void {
    EnvGuard guard(*this, std::move(env));
    for (auto& stmt : block->getStmts()) {
        execute(stmt.get());
    }
}

} // namespace minijsc
