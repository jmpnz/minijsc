//===----------------------------------------------------------------------===//
// BytecodeCompiler.h: Declaration and implementation of minijsc bytecode
// compiler.
//===----------------------------------------------------------------------===//
#ifndef BYTECODE_COMPILER_H
#define BYTECODE_COMPILER_H

#include "AST.h"
#include "Bytecode.h"

#include <cstdint>

namespace minijsc {

/// Bytecode compiler implements the visitor pattern emitting bytecode
/// at each node.
class BytecodeCompiler : public ASTVisitor {
    public:
    /// Default constructor.
    explicit BytecodeCompiler() = default;
    /// Default destructor.
    ~BytecodeCompiler() override = default;

    /// Visit a literal expression.
    auto visitLiteralExpr(JSLiteralExpr* expr) -> void override;
    /// Visit a binary expression.
    auto visitBinaryExpr(JSBinExpr* expr) -> void override;
    /// Visit a unary expression.
    auto visitUnaryExpr(JSUnaryExpr* expr) -> void override;
    /// Visit a grouping expression.
    auto visitGroupingExpr(JSGroupingExpr* expr) -> void override;
    /// Visit a variable expression.
    auto visitVarExpr(JSVarExpr* expr) -> void override;
    /// Visit an assignment expression.
    auto visitAssignExpr(JSAssignExpr* expr) -> void override;
    /// Visit a call expression.
    auto visitCallExpr(JSCallExpr* expr) -> void override;
    /// Visit a block statement.
    auto visitBlockStmt(JSBlockStmt* block) -> void override;
    /// Visit an expression statement.
    auto visitExprStmt(JSExprStmt* stmt) -> void override;
    /// Visit an if statement.
    auto visitIfStmt(JSIfStmt* stmt) -> void override;
    /// Visit a while statement.
    auto visitWhileStmt(JSWhileStmt* stmt) -> void override;
    /// Visit a for statement.
    auto visitForStmt(JSForStmt* stmt) -> void override;
    /// Visit a variable declaration.
    auto visitVarDecl(JSVarDecl* stmt) -> void override;
    /// Visit a function declaration.
    auto visitFuncDecl(JSFuncDecl* stmt) -> void override;
    /// Visit a return statement.
    auto visitReturnStmt(JSReturnStmt* stmt) -> void override;

    private:
    std::vector<uint8_t> byteCodeBuffer;
};

} // namespace minijsc

#endif
