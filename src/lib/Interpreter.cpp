//===----------------------------------------------------------------------===//
// Interpreter.cpp: This file implements the baseline interpreter for minijsc.
// The interpreter implements the visitor pattern declared in AST.h each visit
// method implements an evaluation pattern depending on the visited node kind.
//===----------------------------------------------------------------------===//
#include "AST.h"
#include "Interpreter.h"
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
auto Interpreter::visitExprStmt(JSExprStmt* stmt) -> void {
    evaluate(stmt->getExpr().get());
}

/// Processing If statement nodes start by evaluating the conditional expression
/// depending on the results of evaluation we dispatch execution to either
/// the branch after the if statement or the branch after the else statement.
/// If no else statement is provided we simply return.
auto Interpreter::visitIfStmt(JSIfStmt* stmt) -> void {
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
auto Interpreter::visitWhileStmt(JSWhileStmt* stmt) -> void {
    while (isTruthy(evaluate(stmt->getCondition().get()))) {
        execute(stmt->getBody().get());
    }
}

/// For statements are evaluated similarly to while loops, we first execute
/// the initializer statement which loads declarations or assignments into
/// the execution scope. After the initializer is executed we repeat the same
/// pattern we did for the while loop, only this time after executing the body
/// of the loop we also evaluate the step expression.
auto Interpreter::visitForStmt(JSForStmt* stmt) -> void {
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
auto Interpreter::visitBlockStmt(JSBlockStmt* block) -> void {
    fmt::print("visitBlockStmt\n");
    fmt::print("All pointers are good\n");
    auto env = Environment();
    executeBlock(block, env);
    fmt::print("Done processing block\n");
}

/// Variable declarations can either have an initial value or are assigned
/// the `null` value. Once the assignment expression is evaluated a new
/// binding is defined in the current environment.
auto Interpreter::visitVarDecl(JSVarDecl* stmt) -> void {
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
auto Interpreter::visitFuncDecl(JSFuncDecl* stmt) -> void {
    auto decl = std::make_shared<JSFuncDecl>(*stmt);
    auto func = std::make_shared<JSFunction>(decl);
    define(stmt->getName().getLexeme(), func);
}

/// Variable expressions return the value of the variable we do that by
/// resolving the binding in starting from the current environment, walking
/// bottom up to the top level environment.
auto Interpreter::visitVarExpr(JSVarExpr* expr) -> void {
    fmt::print("Resolving variable expression: {}\n",
               expr->getName().getLexeme());
    // TODO: handle nullopt
    auto resolvedValue = resolve(expr->getName().getLexeme());
    pushValue(resolvedValue);
}

/// Return statements evaluate the return value and unwind the stack back
/// to the call expression.
auto Interpreter::visitReturnStmt(JSReturnStmt* stmt) -> void {
    std::shared_ptr<JSValue> value = nullptr;
    if (stmt->getValue() != nullptr) {
        value = evaluate(stmt->getValue().get());
    }
    throw JSReturn(value);
}

/// Assignment expressions will assign an expression's value to the variable
/// on the left hand side, similarly to variable expression we walk proceed
/// bottom up through the environment scopes. If the variable isn't found
/// a runtime error is thrown.
auto Interpreter::visitAssignExpr(JSAssignExpr* expr) -> void {
    auto value = std::static_pointer_cast<JSBasicValue>(
        evaluate(expr->getValue().get()));
    fmt::print("Visit assign expr: {}\n", value->toString());
    assign(expr->getName().getLexeme(), value);
    pushValue(value);
}

/// Call expressions
auto Interpreter::visitCallExpr(JSCallExpr* expr) -> void {
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
    auto ret    = std::make_shared<JSBasicValue>(result);
    pushValue(ret);
}

/// Literal expressions will simply return the literal value.
auto Interpreter::visitLiteralExpr(JSLiteralExpr* expr) -> void {
    fmt::print("Value : {}\n",
               ((JSBasicValue*)expr->getValue().get())->toString());
    auto value = expr->getValue();
    pushValue(value);
}

/// Binary expressions are processed by evaluating the left and right hand
/// sides of the expression. The rules used for evaluation follow JavaScript's
/// rules, we upcast depending on the values of either sides of the expression.
auto Interpreter::visitBinaryExpr(JSBinExpr* expr) -> void {
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
            auto res = std::make_shared<JSBasicValue>(
                lhs->getValue<JSString>() + rhs->getValue<JSString>());
            pushValue(res);
            break;
        }
        // 2. If both sides are numbers sum them.
        if (lhs->isNumber() && rhs->isNumber()) {
            auto res = std::make_shared<JSBasicValue>(
                lhs->getValue<JSNumber>() + rhs->getValue<JSNumber>());
            pushValue(res);
            break;
        }
        // If one side is a string, cast the other side to a string.
        if (lhs->isString()) {
            auto res = std::make_shared<JSBasicValue>(
                lhs->getValue<JSString>() + rhs->toString());
            pushValue(res);
            break;
        }
        if (rhs->isString()) {
            auto res = std::make_shared<JSBasicValue>(
                lhs->toString() + rhs->getValue<JSString>());
            pushValue(res);
            break;
        }
        // Throw a type error if none of the above.
        throw std::runtime_error(
            "Uncaught type error '+' unsupported for types : " +
            lhs->toString() + " and " + rhs->toString());
    }
    case JSTokenKind::Minus: {
        auto res = std::make_shared<JSBasicValue>(lhs->getValue<JSNumber>() -
                                                  rhs->getValue<JSNumber>());
        pushValue(res);
        break;
    }
    case JSTokenKind::Star: {
        auto res = std::make_shared<JSBasicValue>(lhs->getValue<JSNumber>() *
                                                  rhs->getValue<JSNumber>());
        pushValue(res);
        break;
    }
    case JSTokenKind::Slash: {
        auto res = std::make_shared<JSBasicValue>(lhs->getValue<JSNumber>() /
                                                  rhs->getValue<JSNumber>());
        pushValue(res);
        break;
    }
    // Comparison operations.
    case JSTokenKind::Greater: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() > rhs->getValue<JSNumber>()));
        pushValue(res);
        break;
    }
    case JSTokenKind::GreaterEqual: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() >= rhs->getValue<JSNumber>()));
        pushValue(res);
        break;
    }
    case JSTokenKind::Less: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() < rhs->getValue<JSNumber>()));
        pushValue(res);
        break;
    }
    case JSTokenKind::LessEqual: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() <= rhs->getValue<JSNumber>()));
        pushValue(res);
        break;
    }
    case JSTokenKind::BangEqual: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() != rhs->getValue<JSNumber>()));
        pushValue(res);

        break;
    }
    case JSTokenKind::EqualEqual: {
        auto res = std::make_shared<JSBasicValue>(
            JSBoolean(lhs->getValue<JSNumber>() == rhs->getValue<JSNumber>()));
        pushValue(res);
        break;
    }
    default:
        throw std::invalid_argument("Unknown operator");
    }
}

/// Unary expressions are processed by evaluating the righyt hand side
/// of the expression and executing the operator on the left hand side.
auto Interpreter::visitUnaryExpr(JSUnaryExpr* expr) -> void {
    // Type check before cast since -[1,2,3] might be passed.
    auto rhs = std::static_pointer_cast<JSBasicValue>(
        evaluate(expr->getRight().get()));
    auto unaryOp = expr->getOperator();
    switch (unaryOp.getKind()) {
    case JSTokenKind::Minus: {
        auto res = std::make_shared<JSBasicValue>(-rhs->getValue<JSNumber>());
        pushValue(res);
        break;
    }
    case JSTokenKind::Bang: {
        fmt::print("Unary/!/");
        auto res = std::make_shared<JSBasicValue>(!isTruthy(rhs));
        pushValue(res);
        break;
    }
    default: {
        fmt::print("visitUnaryExpr::nullPtr");
        auto res = std::make_shared<JSBasicValue>(nullptr);
        pushValue(res);
        break;
    }
    }
}

/// Grouping expressions are processed recursively by evaluating the expression
/// in the grouping.
auto Interpreter::visitGroupingExpr(JSGroupingExpr* expr) -> void {
    auto value = evaluate(expr->getExpr().get());
    pushValue(value);
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
        fmt::print("Expr of kind {} is not null\n",
                   astNodeKindToString(expr->getKind()));
        /// When we call accept all expressions we visit will push
        /// a value to the internal value stack, when returning from
        /// the visitor we pop the value back from the stack.
        expr->accept(this);
        return popValue();
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
