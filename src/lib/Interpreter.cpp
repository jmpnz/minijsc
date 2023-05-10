//===----------------------------------------------------------------------===//
// Interpreter.cpp: This file implements the baseline interpreter for minijsc.
//===----------------------------------------------------------------------===//
#include "AST.h"
#include "Interpreter.h"
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

/// Interpreter visits an if statement, evaluating the conditional expression
/// and dispatching execution to either the then or else branch.
auto Interpreter::visitIfStmt(std::shared_ptr<JSIfStmt> stmt) -> void {
    auto expr = evaluate(stmt->getCondition().get());
    if (isTruthy(expr) && (stmt->getThenBranch() != nullptr)) {
        return execute(stmt->getThenBranch().get());
    }
    if (!isTruthy(expr) && (stmt->getElseBranch() != nullptr)) {
        return execute(stmt->getElseBranch().get());
    }
}

/// Interpreter visits a while statement, evaluating the conditional expression
/// and executing the loop body if it evaluates to true.
auto Interpreter::visitWhileStmt(std::shared_ptr<JSWhileStmt> stmt) -> void {
    while (isTruthy(evaluate(stmt->getCondition().get()))) {
        execute(stmt->getBody().get());
    }
}

/// Interpreter visits a block statement executing the statements within
/// the block.
auto Interpreter::visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void {
    fmt::print("visitBlockStmt\n");
    fmt::print("All pointers are good\n");
    auto env = Environment();
    executeBlock(block.get(), env);
    fmt::print("Done processing block\n");
}

/// Interpreter visits variable declarations defining bindings in the runtime
/// environment.
auto Interpreter::visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void {
    auto value = JSBasicValue();
    if (stmt->getInitializer()) {
        value = evaluate(stmt->getInitializer().get());
        fmt::print("Has initializer : {}\n", value.toString());
    }
    fmt::print("Defining binding : {} -> {}\n", stmt->getName(),
               value.toString());
    define(stmt->getName(), value);
}

/// Interpreter visits variable expressions returning the value.
auto Interpreter::visitVarExpr(std::shared_ptr<JSVarExpr> expr)
    -> JSBasicValue {
    fmt::print("Resolving variable expression: {}\n",
               expr->getName().getLexeme());
    // TODO: handle nullopt
    return resolve(expr->getName().getLexeme());
}

/// Interpreter visits assignment expression, evaluating the right handside
/// and assigning it to the left handside.
auto Interpreter::visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
    -> JSBasicValue {
    auto value = evaluate(expr->getValue().get());
    fmt::print("Visit assign expr: {}\n", value.toString());
    assign(expr->getName().getLexeme(), value);
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
    case JSTokenKind::Plus: {
        // Overloading for the plus operator:
        // 1. If both sides are strings concatenate them.
        if (lhs.isString() && rhs.isString()) {
            return {lhs.getValue<JSString>() + rhs.getValue<JSString>()};
        }
        // 2. If both sides are numbers sum them.
        if (lhs.isNumber() && rhs.isNumber()) {
            return {lhs.getValue<JSNumber>() + rhs.getValue<JSNumber>()};
        }
        // If one side is a string, cast the other side to a string.
        if (lhs.isString()) {
            return {lhs.getValue<JSString>() + rhs.toString()};
        }
        if (rhs.isString()) {
            return {lhs.toString() + rhs.getValue<JSString>()};
        }
        // Throw a type error if none of the above.
        throw std::runtime_error(
            "Uncaught type error '+' unsupported for types : " +
            lhs.toString() + " and " + rhs.toString());
    }
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
        // TODO: move try/catch to outer scope
        throw e;
    }
}

/// Evaluate expressions.
auto Interpreter::evaluate(JSExpr* expr) -> JSBasicValue {
    return expr->accept(this);
}

/// Execute a single statement.
auto Interpreter::execute(JSStmt* stmt) -> void { return stmt->accept(this); }

/// Execute a block.
auto Interpreter::executeBlock(JSBlockStmt* block, Environment env) -> void {
    // Get the current scope.
    auto currentScopeIdx = this->currIdx;
    // Create a new scope, with the current scope as its parent.
    // auto newScopedEnv = Environment(Parent = currentScopeIdx);
    // Set the parent pointer on the `env` scope to the current scope index.
    env.setParentPtr(currentScopeIdx);
    // Append the new environment to the nested envs and increment the currIdx.
    this->symTables.emplace_back(env);
    // Substract one since we started from a current scope ptr of 0 because
    // vectors are 0-indexed.
    currIdx += 1;
    fmt::print("Scope index: {}\n", currIdx);
    for (auto& stmt : block->getStmts()) {
        // If execute fails we unwind the new environment and restore
        // back the previous index
        execute(stmt.get());
    }
    // saveguard parent scope ptr before destroying the current scope
    auto parent = this->symTables[currIdx].getParentPtr();
    // pop the env from the stack
    this->symTables.pop_back();
    // restore old stack pointer
    this->currIdx = parent;
}

} // namespace minijsc
