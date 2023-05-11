//===----------------------------------------------------------------------===//tests
// Interpreter.cpp: This file implements the baseline bytecode compiler
// for minijsc.
// The bytecode compiler implements the visitor pattern declared in AST.h
// each visit method emits bytecode depending on the visited node kind.
//===----------------------------------------------------------------------===//

#include "BytecodeCompiler.h"
#include "AST.h"
#include "JSValue.h"
#include "fmt/core.h"
#include <memory>

namespace minijsc {

auto BytecodeCompiler::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> std::shared_ptr<JSValue> {

    auto value = std::static_pointer_cast<JSBasicValue>(expr->getValue());

    fmt::print("Emitting constant literal {}\n", value->toString());
    fmt::print("OpConstant\n");
    return nullptr;
}

/// Visit a binary expression.
auto BytecodeCompiler::visitBinaryExpr(std::shared_ptr<JSBinExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a unary expression.
auto BytecodeCompiler::visitUnaryExpr(std::shared_ptr<JSUnaryExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a grouping expression.
auto BytecodeCompiler::visitGroupingExpr(
    std::shared_ptr<JSGroupingExpr> /*expr*/) -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a variable expression.
auto BytecodeCompiler::visitVarExpr(std::shared_ptr<JSVarExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit an assignment expression.
auto BytecodeCompiler::visitAssignExpr(std::shared_ptr<JSAssignExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a call expression.
auto BytecodeCompiler::visitCallExpr(std::shared_ptr<JSCallExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a block statement.
auto BytecodeCompiler::visitBlockStmt(std::shared_ptr<JSBlockStmt> block)
    -> void {}

/// Visit an expression statement.
auto BytecodeCompiler::visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void {
}

/// Visit an if statement.
auto BytecodeCompiler::visitIfStmt(std::shared_ptr<JSIfStmt> stmt) -> void {}

/// Visit a while statement.
auto BytecodeCompiler::visitWhileStmt(std::shared_ptr<JSWhileStmt> stmt)
    -> void {}

/// Visit a for statement.
auto BytecodeCompiler::visitForStmt(std::shared_ptr<JSForStmt> stmt) -> void {}

/// Visit a variable declaration.
auto BytecodeCompiler::visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void {}

/// Visit a function declaration.
auto BytecodeCompiler::visitFuncDecl(std::shared_ptr<JSFuncDecl> stmt) -> void {
}

/// Visit a return statement.
auto BytecodeCompiler::visitReturnStmt(std::shared_ptr<JSReturnStmt> stmt)
    -> void {}

} // namespace minijsc
