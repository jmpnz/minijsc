//===----------------------------------------------------------------------===//
//
// ASTOptimizer.h: This file defines classes for representing AST level
// optimizers.
// All optimizers implement the visitor pattern define in the `AST.h`.
// The currently implemented optimizers are :
//
// ConstantFoldingOptimizer: visits binary and unary expressions and executes
// constant folding optimizations.
//===----------------------------------------------------------------------===//
#ifndef AST_OPTIMIZER_H
#define AST_OPTIMIZER_H
#include "AST.h"

namespace minijsc {

/// Constant folding optimizer implements constant folding optimizations
/// at the AST level on binary and unary expressions.
class ConstantFoldingOptimizer {
    /// Default constructor with no arguments.
    explicit ConstantFoldingOptimizer() = default;

    /// rewriteAST is the core method of all our optimizers, in this case
    /// the optimizer resolves any constant literals in unary or binary
    /// expressions and executes a folding pass.
    auto rewriteAST(JSExpr* expr) -> void;
};

} // namespace minijsc

#endif
