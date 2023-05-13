//===----------------------------------------------------------------------===//tests
// Interpreter.cpp: This file implements the baseline bytecode compiler
// for minijsc.
// The bytecode compiler implements the visitor pattern declared in AST.h
// each visit method emits bytecode depending on the visited node kind.
//===----------------------------------------------------------------------===//

#include "AST.h"
#include "BytecodeCompiler.h"
#include "JSValue.h"
#include "fmt/core.h"
#include <memory>

namespace minijsc {

auto BytecodeCompiler::visitLiteralExpr(JSLiteralExpr* expr) -> void {

    auto value = std::static_pointer_cast<JSBasicValue>(expr->getValue());

    fmt::print("Emitting constant literal {}\n", value->toString());
    fmt::print("OpConstant\n");
}

/// Visit a binary expression.
auto BytecodeCompiler::visitBinaryExpr(JSBinExpr* /*expr*/) -> void {}

/// Visit a unary expression.
auto BytecodeCompiler::visitUnaryExpr(JSUnaryExpr* /*expr*/) -> void {}

/// Visit a grouping expression.
auto BytecodeCompiler::visitGroupingExpr(JSGroupingExpr* /*expr*/) -> void {}

/// Visit a variable expression.
auto BytecodeCompiler::visitVarExpr(JSVarExpr* /*expr*/) -> void {}

/// Visit an assignment expression.
auto BytecodeCompiler::visitAssignExpr(JSAssignExpr* /*expr*/) -> void {}

/// Visit a call expression.
auto BytecodeCompiler::visitCallExpr(JSCallExpr* /*expr*/) -> void {}

/// Visit a block statement.
auto BytecodeCompiler::visitBlockStmt(JSBlockStmt* block) -> void {}

/// Visit an expression statement.
auto BytecodeCompiler::visitExprStmt(JSExprStmt* stmt) -> void {}

/// Visit an if statement.
auto BytecodeCompiler::visitIfStmt(JSIfStmt* stmt) -> void {}

/// Visit a while statement.
auto BytecodeCompiler::visitWhileStmt(JSWhileStmt* stmt) -> void {}

/// Visit a for statement.
auto BytecodeCompiler::visitForStmt(JSForStmt* stmt) -> void {}

/// Visit a variable declaration.
auto BytecodeCompiler::visitVarDecl(JSVarDecl* stmt) -> void {}

/// Visit a function declaration.
auto BytecodeCompiler::visitFuncDecl(JSFuncDecl* stmt) -> void {}

/// Visit a return statement.
auto BytecodeCompiler::visitReturnStmt(JSReturnStmt* stmt) -> void {}

} // namespace minijsc
