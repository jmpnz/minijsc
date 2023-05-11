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
class BytecodeCompiler : public Visitor {
    public:
    /// Default constructor.
    explicit BytecodeCompiler() = default;
    /// Default destructor.
    ~BytecodeCompiler() override = default;

    /// Visit a literal expression.
    auto visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a binary expression.
    auto visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a unary expression.
    auto visitUnaryExpr(std::shared_ptr<JSUnaryExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a grouping expression.
    auto visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a variable expression.
    auto visitVarExpr(std::shared_ptr<JSVarExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit an assignment expression.
    auto visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a call expression.
    auto visitCallExpr(std::shared_ptr<JSCallExpr> expr)
        -> std::shared_ptr<JSValue> override;
    /// Visit a block statement.
    auto visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void override;
    /// Visit an expression statement.
    auto visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void override;
    /// Visit an if statement.
    auto visitIfStmt(std::shared_ptr<JSIfStmt> stmt) -> void override;
    /// Visit a while statement.
    auto visitWhileStmt(std::shared_ptr<JSWhileStmt> stmt) -> void override;
    /// Visit a for statement.
    auto visitForStmt(std::shared_ptr<JSForStmt> stmt) -> void override;
    /// Visit a variable declaration.
    auto visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void override;
    /// Visit a function declaration.
    auto visitFuncDecl(std::shared_ptr<JSFuncDecl> stmt) -> void override;
    /// Visit a return statement.
    auto visitReturnStmt(std::shared_ptr<JSReturnStmt> stmt) -> void override;

    private:
    std::vector<uint8_t> buffer;
};

} // namespace minijsc

#endif
