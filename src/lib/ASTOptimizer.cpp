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
 * ASTOptimizer::rewrite(JSExpr* expr) -> std::shared_ptr<JSExpr> {
 *  if (expr) {
 *      expr->accept(this);
 *      // Assume expr is a binary expression
 *      // expr = expr->accept(this) <==> this->visitBinaryExpression(expr)
 *      // When rewritting I need to take ownership of the passed expression
 *      // Create a new one in place and return the new expressio When rewritting I need to take
 *      ownership of the passed expression
 *      // Create a new one in place and return the new expressionn
 *      // Which does constant fold optimization
 *      // AST::VisitBinaryExpression(BinExpr expr) {
 *      // auto rewrittenExpr = fold(expr)
 *      //
 *      //
 *      //}
 *  }
 * }
 *
 * */

auto ASTOptimizer::rewriteAST(std::shared_ptr<JSExpr> expr)
    -> std::shared_ptr<JSExpr> {
    auto kind = expr->getKind();
    fmt::print("Node kind: {}\n", astNodeKindToString(kind));
    /// Visit left and right nodes to fold them.
    auto binExpr = std::static_pointer_cast<JSBinExpr>(expr);
    // Visiting binary expressions will either push the folded value into
    // the stack or unfolded.
    visitBinaryExpr(binExpr);
    auto folded = expressionStack.back();
    expressionStack.pop_back();
    return folded;
}

auto ASTOptimizer::visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
    -> void {}

/// Visit a binary expression.
auto ASTOptimizer::visitBinaryExpr(std::shared_ptr<JSBinExpr> expr) -> void {

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
                // push folded expression into the stack.
                expressionStack.emplace_back(std::make_shared<JSLiteralExpr>(
                    std::make_shared<JSBasicValue>(litVal)));
            }
        }
    // If no optimization was made push the original value into the expression stack.
    expressionStack.emplace_back(expr);
}

/// Visit a unary expression.
auto ASTOptimizer::visitUnaryExpr(std::shared_ptr<JSUnaryExpr> /*expr*/)
    -> void {}

/// Visit a grouping expression.
auto ASTOptimizer::visitGroupingExpr(std::shared_ptr<JSGroupingExpr> /*expr*/)
    -> void {}

/// Visit a variable expression.
auto ASTOptimizer::visitVarExpr(std::shared_ptr<JSVarExpr> /*expr*/) -> void {}

/// Visit an assignment expression.
auto ASTOptimizer::visitAssignExpr(std::shared_ptr<JSAssignExpr> /*expr*/)
    -> void {}

/// Visit a call expression.
auto ASTOptimizer::visitCallExpr(std::shared_ptr<JSCallExpr> /*expr*/) -> void {

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
