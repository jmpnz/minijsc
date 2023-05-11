//===----------------------------------------------------------------------===//
// ASTOptimizer.h: This file defines classes for representing AST level
// optimizers.
// All optimizers implement the visitor pattern define in the `AST.h`.
// The currently implemented optimizers are :
//
// ASTOptimizer : visits binary and unary expressions and executes
// constant folding optimizations.
//===----------------------------------------------------------------------===//
#include "AST.h"
#include "ASTOptimizer.h"

namespace minijsc {

/*
class ConstantFoldingOptimizer : public AstOptimizer {
public:
  void visit(BinaryExpression &node) override {
    // Visit left and right children to fold constants
    visit(node.left);
    visit(node.right);

    // Check if both children are literals
    if (auto left_lit = dynamic_cast<LiteralExpression*>(node.left.get())) {
      if (auto right_lit = dynamic_cast<LiteralExpression*>(node.right.get())) {
        // Fold the binary expression
        switch (node.op) {
          case BinaryOperator::Plus:
            foldAddition(left_lit, right_lit, node);
            break;
          case BinaryOperator::Minus:
            foldSubtraction(left_lit, right_lit, node);
            break;
          case BinaryOperator::Times:
            foldMultiplication(left_lit, right_lit, node);
            break;
          case BinaryOperator::Divide:
            foldDivision(left_lit, right_lit, node);
            break;
          // Add more cases for other binary operators as needed
          default:
            break;
        }
      }
    }
  }

private:
 *
 *
 * */

auto ASTOptimizer::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a binary expression.
auto ASTOptimizer::visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
    -> std::shared_ptr<JSValue> {

    /// Visit left and right nodes to fold them.
    auto left  = expr->getLeft();
    auto right = expr->getRight();

    // Check if the children are literals
    if (auto leftRef = dynamic_cast<JSLiteralExpr*>(left.get()))
        if (auto rightRef = dynamic_cast<JSLiteralExpr*>(right.get())) {
            // If the operation is addition, do a fold on the addition expression.
            auto op = expr->getOperator();
            ;
            if (op.getKind() == JSTokenKind::Plus) {
                auto leftVal =
                    std::static_pointer_cast<JSBasicValue>(leftRef->getValue());
                auto rightVal = std::static_pointer_cast<JSBasicValue>(
                    rightRef->getValue());
                auto litVal = leftVal->getValue<JSNumber>() +
                              rightVal->getValue<JSNumber>();
                fmt::print("Folded value : {}\n", litVal);
            }
        }

    return nullptr;
}

/// Visit a unary expression.
auto ASTOptimizer::visitUnaryExpr(std::shared_ptr<JSUnaryExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a grouping expression.
auto ASTOptimizer::visitGroupingExpr(std::shared_ptr<JSGroupingExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a variable expression.
auto ASTOptimizer::visitVarExpr(std::shared_ptr<JSVarExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit an assignment expression.
auto ASTOptimizer::visitAssignExpr(std::shared_ptr<JSAssignExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a call expression.
auto ASTOptimizer::visitCallExpr(std::shared_ptr<JSCallExpr> /*expr*/)
    -> std::shared_ptr<JSValue> {
    return nullptr;
}

/// Visit a block statement.
auto ASTOptimizer::visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void {}

/// Visit an expression statement.
auto ASTOptimizer::visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void {}

/// Visit an if statement.
auto ASTOptimizer::visitIfStmt(std::shared_ptr<JSIfStmt> stmt) -> void {}

/// Visit a while statement.
auto ASTOptimizer::visitWhileStmt(std::shared_ptr<JSWhileStmt> stmt) -> void {}

/// Visit a for statement.
auto ASTOptimizer::visitForStmt(std::shared_ptr<JSForStmt> stmt) -> void {}

/// Visit a variable declaration.
auto ASTOptimizer::visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void {}

/// Visit a function declaration.
auto ASTOptimizer::visitFuncDecl(std::shared_ptr<JSFuncDecl> stmt) -> void {}

/// Visit a return statement.
auto ASTOptimizer::visitReturnStmt(std::shared_ptr<JSReturnStmt> stmt) -> void {
}

} // namespace minijsc
