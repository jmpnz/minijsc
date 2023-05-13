//===----------------------------------------------------------------------===//
// ASTOptimizer.h: This file defines classes for representing AST level
// optimizers.
// All optimizers implement the visitor pattern define in the `AST.h`.
// The currently implemented optimizers are :
//
// ASTOptimizer: visits binary and unary expressions and executes
// constant folding optimizations.
//===----------------------------------------------------------------------===//
#ifndef ASTOPTIMIZER_H
#define ASTOPTIMIZER_H

#include "AST.h"
#include <vector>

namespace minijsc {

/// Constant folding optimizer implements constant folding optimizations
/// at the AST level on binary and unary expressions.
class ASTOptimizer : public ASTVisitor {
    public:
    /// Default constructor with no arguments.
    explicit ASTOptimizer() = default;

    /// rewriteAST is the core method of all our optimizers, in this case
    /// the optimizer resolves any constant literals in unary or binary
    /// expressions and executes a folding pass.
    auto rewriteAST(std::shared_ptr<JSExpr> expr) -> std::shared_ptr<JSExpr>;

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
    /// Expression stack is used to track down optimized expressions.
    std::vector<std::shared_ptr<JSExpr>> expressionStack;
};

} // namespace minijsc

#endif
