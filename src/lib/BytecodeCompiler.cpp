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

#include <cassert>
#include <memory>

namespace minijsc {

auto BytecodeCompiler::visitLiteralExpr(JSLiteralExpr* expr) -> void {
    if (expr == nullptr) {
        emit(OPCode::Constant, JSBasicValue());
        return;
    }
    auto value = std::static_pointer_cast<JSBasicValue>(expr->getValue());
    emit(OPCode::Constant, *value);
    // emit(OPCode::Return);
    fmt::print("Emitting constant literal {}\n", value->toString());
    fmt::print("OpConstant\n");
}

/// Visit a binary expression.
auto BytecodeCompiler::visitBinaryExpr(JSBinExpr* expr) -> void {
    auto binOp = expr->getOperator();
    compile(expr->getLeft().get());
    compile(expr->getRight().get());
    switch (binOp.getKind()) {
    case JSTokenKind::Plus: {
        emit(OPCode::Add);
        break;
    }
    case JSTokenKind::Minus: {
        emit(OPCode::Sub);
        break;
    }
    case JSTokenKind::Star: {
        emit(OPCode::Mul);
        break;
    }
    case JSTokenKind::Slash: {
        emit(OPCode::Div);
        break;
    }
    case JSTokenKind::BangEqual: {
        emit(OPCode::NotEqual);
        break;
    }
    case JSTokenKind::EqualEqual: {
        emit(OPCode::Equal);
        break;
    }
    case JSTokenKind::Less: {
        emit(OPCode::Lesser);
        break;
    }
    case JSTokenKind::Greater: {
        emit(OPCode::Greater);
        break;
    }
    case JSTokenKind::GreaterEqual: {
        emit(OPCode::GreaterEqual);
        break;
    }
    case JSTokenKind::LessEqual: {
        emit(OPCode::LesserEqual);
        break;
    }
    default:
        fmt::print("Unknown binary operation\n");
        break;
    }
}

/// Visit a unary expression.
auto BytecodeCompiler::visitUnaryExpr(JSUnaryExpr* expr) -> void {
    auto unaryOp = expr->getOperator();
    compile(expr->getRight().get());
    switch (unaryOp.getKind()) {
    case JSTokenKind::Minus:
        emit(OPCode::Negate);
        break;
    case JSTokenKind::Bang:
        emit(OPCode::Not);
        break;
    default:
        fmt::print("Unknown binary operation\n");
        break;
    }
}

/// Visit a logical expression.
auto BytecodeCompiler::visitLogicalExpr(JSLogicalExpr* expr) -> void {
    auto binOp = expr->getOperator();
    compile(expr->getLeft().get());
    compile(expr->getRight().get());
    switch (binOp.getKind()) {
    case JSTokenKind::Or: {
        emit(OPCode::Or);
        break;
    }
    case JSTokenKind::And: {
        emit(OPCode::And);
        break;
    }
    default:
        fmt::print("Unknown logical operation\n");
        break;
    }
}

/// Visit a grouping expression.
auto BytecodeCompiler::visitGroupingExpr(JSGroupingExpr* expr) -> void {
    compile(expr->getExpr().get());
}

/// Visit a variable expression.
auto BytecodeCompiler::visitVarExpr(JSVarExpr* expr) -> void {}

/// Visit an assignment expression.
auto BytecodeCompiler::visitAssignExpr(JSAssignExpr* exor) -> void {}

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
auto BytecodeCompiler::visitVarDecl(JSVarDecl* stmt) -> void {
    auto ident = stmt->getName();
    if (stmt->getInitializer().get() != nullptr) {
        compile(stmt->getInitializer().get());
    } else {
        // If no assignment then set it as undefined.
        emit(OPCode::Constant, JSBasicValue());
    }
    emit(OPCode::SetGlobal, ident);
}

/// Visit a function declaration.
auto BytecodeCompiler::visitFuncDecl(JSFuncDecl* stmt) -> void {}

/// Visit a return statement.
auto BytecodeCompiler::visitReturnStmt(JSReturnStmt* stmt) -> void {
    emit(OPCode::Return);
}

} // namespace minijsc
