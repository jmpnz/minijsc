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
    std::shared_ptr<JSBasicValue> value;
    if (stmt->getInitializer()) {
        value = static_pointer_cast<JSBasicValue>(
            evaluate(stmt->getInitializer().get()));
        fmt::print("Has initializer : {}\n", value->toString());
    }
    fmt::print("Defining binding : {} -> {}\n", stmt->getName(),
               value->toString());
    define(stmt->getName(), value);
}

/// Function declarations create a binding to a function.
auto Interpreter::visitFuncDecl(std::shared_ptr<JSFuncDecl> stmt) -> void {
    auto func = std::make_shared<JSFunction>(stmt);
    define(stmt->getName().getLexeme(), func);
}

/// Variable expressions return the value of the variable we do that by
/// resolving the binding in starting from the current environment, walking
/// bottom up to the top level environment.
auto Interpreter::visitVarExpr(std::shared_ptr<JSVarExpr> expr)
    -> std::shared_ptr<JSValue> {
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
    -> std::shared_ptr<JSValue> {
    auto value = std::static_pointer_cast<JSBasicValue>(
        evaluate(expr->getValue().get()));
    fmt::print("Visit assign expr: {}\n", value->toString());
    assign(expr->getName().getLexeme(), value);
    return value;
}

/// Call expressions
auto Interpreter::visitCallExpr(std::shared_ptr<JSCallExpr> expr)
    -> std::shared_ptr<JSValue> {
    // Evaluate should return std::shared_ptr<JSValue>
    auto callee = evaluate(expr->getCallee().get());
    // Casting shouldn't be required here.
    assert(callee != nullptr);
    std::vector<JSBasicValue> args;
    for (auto& arg : expr->getArgs()) {
        auto value =
            std::static_pointer_cast<JSBasicValue>(evaluate(arg.get()));
        args.emplace_back(*value);
    }
    auto func = std::static_pointer_cast<JSFunction>(callee);
    assert(func != nullptr);
    auto result = func->call(this, args);
    return std::make_shared<JSBasicValue>(result);
}

/// Literal expressions will simply return the literal value.
auto Interpreter::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> std::shared_ptr<JSValue> {
    return expr->getValue();
}

/// Binary expressions are processed by evaluating the left and right hand
/// sides of the expression. The rules used for evaluation follow JavaScript's
/// rules, we upcast depending on the values of either sides of the expression.
auto Interpreter::visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
    -> std::shared_ptr<JSValue> {
    auto lhs =
        std::static_pointer_cast<JSBasicValue>(evaluate(expr->getLeft().get()));
    auto rhs = std::static_pointer_cast<JSBasicValue>(
        evaluate(expr->getRight().get()));

    auto binOp = expr->getOperator();

    switch (binOp.getKind()) {
    case JSTokenKind::Plus: {
        // Overloading for the plus operator:
        // 1. If both sides are strings concatenate them.
        if (lhs->isString() && rhs->isString()) {
            auto res = lhs->getValue<JSString>() + rhs->getValue<JSString>();
            return std::make_shared<JSBasicValue>(res);
        }
        // 2. If both sides are numbers sum them.
        if (lhs->isNumber() && rhs->isNumber()) {
            auto res = lhs->getValue<JSNumber>() + rhs->getValue<JSNumber>();
            return std::make_shared<JSBasicValue>(res);
        }
        // If one side is a string, cast the other side to a string.
        if (lhs->isString()) {
            auto res = lhs->getValue<JSString>() + rhs->toString();
            return std::make_shared<JSBasicValue>(res);
        }
        if (rhs->isString()) {
            auto res = lhs->toString() + rhs->getValue<JSString>();
            return std::make_shared<JSBasicValue>(res);
        }
        // Throw a type error if none of the above.
        throw std::runtime_error(
            "Uncaught type error '+' unsupported for types : " +
            lhs->toString() + " and " + rhs->toString());
    }
    case JSTokenKind::Minus: {
        auto res = lhs->getValue<JSNumber>() - rhs->getValue<JSNumber>();
        return std::make_shared<JSBasicValue>(res);
    }
    case JSTokenKind::Star: {
        auto res = lhs->getValue<JSNumber>() * rhs->getValue<JSNumber>();
        return std::make_shared<JSBasicValue>(res);
    }
    case JSTokenKind::Slash: {
        auto res = lhs->getValue<JSNumber>() / rhs->getValue<JSNumber>();
        return std::make_shared<JSBasicValue>(res);
    }
    // Comparison operations.
    case JSTokenKind::Greater:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() > rhs->getValue<JSNumber>()));
    case JSTokenKind::GreaterEqual:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() >= rhs->getValue<JSNumber>()));
    case JSTokenKind::Less:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() < rhs->getValue<JSNumber>()));
    case JSTokenKind::LessEqual:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() <= rhs->getValue<JSNumber>()));
    case JSTokenKind::BangEqual:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() != rhs->getValue<JSNumber>()));
    case JSTokenKind::EqualEqual:
        return std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() == rhs->getValue<JSNumber>()));
    default:
        throw std::invalid_argument("Unknown operator");
    }
}

/// Unary expressions are processed by evaluating the righyt hand side
/// of the expression and executing the operator on the left hand side.
auto Interpreter::visitUnaryExpr(std::shared_ptr<JSUnaryExpr> expr)
    -> std::shared_ptr<JSValue> {
    // Type check before cast since -[1,2,3] might be passed.
    auto rhs = std::static_pointer_cast<JSBasicValue>(
        evaluate(expr->getRight().get()));
    auto unaryOp = expr->getOperator();
    switch (unaryOp.getKind()) {
    case JSTokenKind::Minus:
        return std::make_shared<JSBasicValue>(-rhs->getValue<JSNumber>());
    case JSTokenKind::Bang:
        fmt::print("Unary/!/");
        return std::make_shared<JSBasicValue>(!isTruthy(rhs));
    default:
        fmt::print("visitUnaryExpr::nullPtr");
        return std::make_shared<JSBasicValue>(nullptr);
    }
}

/// Grouping expressions are processed recursively by evaluating the expression
/// in the grouping.
auto Interpreter::visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
    -> std::shared_ptr<JSValue> {
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
auto Interpreter::evaluate(JSExpr* expr) -> std::shared_ptr<JSValue> {
    if (expr != nullptr) {
        fmt::print("Expr is not null\n");
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
