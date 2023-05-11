//===----------------------------------------------------------------------===//
// Interpreter.cpp: This file implements the baseline interpreter for minijsc.
// The interpreter implements the visitor pattern declared in AST.h each visit
// method implements an evaluation pattern depending on the visited node kind.
//===----------------------------------------------------------------------===//
#include "Interpreter.h"
#include "AST.h"
#include "JSCallable.h"
#include "JSRuntime.h"
#include "JSToken.h"
#include "JSValue.h"
#include "fmt/core.h"
#include <cassert>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace minijsc {

/// Expression statement nodes are evaluated when visited.
auto Interpreter::visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void {
    evaluate(stmt->getExpr().get());
}

/// Processing If statement nodes start by evaluating the conditional expression
/// depending on the results of evaluation we dispatch execution to either
/// the branch after the if statement or the branch after the else statement.
/// If no else statement is provided we simply return.
auto Interpreter::visitIfStmt(std::shared_ptr<JSIfStmt> stmt) -> void {
    auto expr = evaluate(stmt->getCondition().get());
    if (isTruthy(expr) && (stmt->getThenBranch() != nullptr)) {
        return execute(stmt->getThenBranch().get());
    }
    if (!isTruthy(expr) && (stmt->getElseBranch() != nullptr)) {
        return execute(stmt->getElseBranch().get());
    }
}

/// While statement nodes are evaluated in two steps. First we evaluate the
/// conditional expression, this needs to be done on each iteration and can't
/// be optimized away. Once the conditional expression is evaluated we check
/// its truth value. As long as the condition evaluates to `true` we execute
/// the statements in the loop body.
auto Interpreter::visitWhileStmt(std::shared_ptr<JSWhileStmt> stmt) -> void {
    while (isTruthy(evaluate(stmt->getCondition().get()))) {
        execute(stmt->getBody().get());
    }
}

/// For statements are evaluated similarly to while loops, we first execute
/// the initializer statement which loads declarations or assignments into
/// the execution scope. After the initializer is executed we repeat the same
/// pattern we did for the while loop, only this time after executing the body
/// of the loop we also evaluate the step expression.
auto Interpreter::visitForStmt(std::shared_ptr<JSForStmt> stmt) -> void {
    // Execute the initializer statement, which will either create the variable
    // or the assignment.
    execute(stmt->getInitializer().get());
    // Evaluate the condition.
    while (isTruthy(evaluate(stmt->getCondition().get()))) {
        // Execute the statement block.
        execute(stmt->getBody().get());
        // Execute the step.
        evaluate(stmt->getStep().get());
    }
}

/// Block statements require us to define a new scope, predefined by curly
/// braces. On each entry to a block statement we create a new environment
/// for the inner scope that references the top level environment scope.
/// After exiting the block statement the inner scope environment is cleaned
/// up and the pointer to the current environment is restored.
auto Interpreter::visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void {
    fmt::print("visitBlockStmt\n");
    fmt::print("All pointers are good\n");
    auto env = Environment();
    executeBlock(block.get(), env);
    fmt::print("Done processing block\n");
}

/// Variable declarations can either have an initial value or are assigned
/// the `null` value. Once the assignment expression is evaluated a new
/// binding is defined in the current environment.
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

/// Variable expressions return the value of the variable we do that by
/// resolving the binding in starting from the current environment, walking
/// bottom up to the top level environment.
auto Interpreter::visitVarExpr(std::shared_ptr<JSVarExpr> expr)
    -> JSBasicValue {
    fmt::print("Resolving variable expression: {}\n",
               expr->getName().getLexeme());
    // TODO: handle nullopt
    return resolve(expr->getName().getLexeme());
}

/// Assignment expressions will assign an expression's value to the variable
/// on the left hand side, similarly to variable expression we walk proceed
/// bottom up through the environment scopes. If the variable isn't found
/// a runtime error is thrown.
auto Interpreter::visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
    -> JSBasicValue {
    auto value = evaluate(expr->getValue().get());
    fmt::print("Visit assign expr: {}\n", value.toString());
    assign(expr->getName().getLexeme(), value);
    return value;
}

/// Call expressions
auto Interpreter::visitCallExpr(std::shared_ptr<JSCallExpr> expr)
    -> JSBasicValue {
    auto callee        = evaluate(expr->getCallee().get());
    JSValue* calleePtr = &callee;
    std::vector<JSBasicValue> args;
    for (auto& arg : expr->getArgs()) {
        args.emplace_back(evaluate(arg.get()));
    }
    auto* func = dynamic_cast<JSCallable*>(calleePtr);
    return func->call(this, args);
}

/// Literal expressions will simply return the literal value.
auto Interpreter::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> JSBasicValue {
    return expr->getValue();
}

/// Binary expressions are processed by evaluating the left and right hand
/// sides of the expression. The rules used for evaluation follow JavaScript's
/// rules, we upcast depending on the values of either sides of the expression.
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

/// Unary expressions are processed by evaluating the righyt hand side
/// of the expression and executing the operator on the left hand side.
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

/// Grouping expressions are processed recursively by evaluating the expression
/// in the grouping.
auto Interpreter::visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
    -> JSBasicValue {
    return evaluate(expr->getExpr().get());
}

/// Interpreter core loop, takes a program which is a sequence of statements
/// executing them one by one.
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

/// Evaluating expressions calls the accept method on the passed expressions.
auto Interpreter::evaluate(JSExpr* expr) -> JSBasicValue {
    if (expr != nullptr) {
        return expr->accept(this);
    }
    return {};
}

/// Executing statements calls the accept method on the passed statement type.
auto Interpreter::execute(JSStmt* stmt) -> void {
    if (stmt != nullptr) {
        stmt->accept(this);
    }
}

/// Executing block statements creates a scopped environment that points at
/// the top level environment cleaning up the stack after execution is finished
/// and restoring the pointer to the current environment.
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
