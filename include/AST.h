//===----------------------------------------------------------------------===//
//
// AST.h: This file defines classes and interfaces for representing AST nodes.
// There are three core AST nodes (declarations, expressions and statements).
//
// - Declarations are symbol assignments such as constants, variables
//   or functions.
// - Statements can be declarations, conditionals, loops or function returns.
// - Expressions are produced values.
//===----------------------------------------------------------------------===//
#ifndef AST_H
#define AST_H
#include <cstddef>
#include <memory>
#include <utility>

#include "JSToken.h"
#include "JSValue.h"

namespace minijsc {

// AST node kinds.
enum class ASTNodeKind {
    LiteralExpr,
    BinaryExpr,
    UnaryExpr,
};

template <typename R> struct Visitor {
    virtual ~Visitor() = default;
    // virtual auto visitAssignExpr(Assign expr) -> R     = 0;
    // virtual auto visitBinaryExpr(Binary expr) -> R     = 0;
    // virtual auto visitCallExpr(Call expr) -> R         = 0;
    // virtual auto visitGetExpr(Get expr) -> R           = 0;
    // virtual auto visitGroupingExpr(Grouping expr) -> R = 0;
    // virtual auto visitLiteralExpr(Literal expr) -> R   = 0;
    // virtual auto visitLogicalExpr(Logical expr) -> R   = 0;
    // virtual auto visitSetExpr(Set expr) -> R           = 0;
    // virtual auto visitSuperExpr(Super expr) -> R       = 0;
    // virtual auto visitThisExpr(This expr) -> R         = 0;
    // virtual auto visitUnaryExpr(Unary expr) -> R       = 0;
    // virtual auto visitVariableExpr(Variable expr) -> R = 0;
};

template <typename R> class Expr {
    public:
    virtual ~Expr() = default;

    // virtual auto accept(Visitor<R>& visitor) -> R = 0;
    virtual auto getKind() -> ASTNodeKind = 0;

    // Nested Expr classes here...
};

/// ASTVisitor defines an interface for a visitor pattern.
// template <typename T> class ASTVisitor {
//    public:
//    virtual ~ASTVisitor()       = default;
//   auto visitBinaryExpr() -> T = 0;
// };

/// Statement base interface.
// class JSStmt {
//    public:
//    virtual ~JSStmt() = default;
// };

/// Expression base interface, the AST is built and evaluated using the visitor
/// pattern. The base JSExpr type defines all possible JavaScript expressions
/// each consumer of the ASTm defines the behavior of how a JSExpr is processed
/// One example would be how the ASTPrinter, Interpreter and Compiler can all
/// consume an AST which is a sequence of statements.
/// When we call ASTPrinter.visitBinaryExpr(expr) the expression is printed
/// to stdout.
/// When we call Interpreter.visitBinaryExpr(expr) the expression is evaluated
/// on the fly instead.
/// And finally if we call Compiler.Compile(expr) the expression is compiled
/// to bytecode.
// template <typename T> class JSExpr {
//     public:
//     // virtual auto visitBinaryExpr(JSBinExpr expr) -> JSBasicValue = 0;
//     virtual ~JSExpr()                                = default;
//     virtual auto accept(ASTVisitor<T>* visitor) -> T = 0;
// };

/// Binary expression implementation.
class JSBinExpr : public Expr<JSBasicValue> {
    public:
    /// Binary expression constructor takes both sides of the expression
    /// and their operand.
    explicit JSBinExpr(Expr* left, JSToken binOp, Expr* right)
        : left(left), right(right), binOp(std::move(binOp)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::BinaryExpr; }

    private:
    // Left handside of the binary operation.
    std::unique_ptr<Expr> left;
    // Right handside of the binary operation.
    std::unique_ptr<Expr> right;
    // Binary operator.
    JSToken binOp;
};

// Unary expression implementation.
class JSUnaryExpr : public Expr<JSBasicValue> {
    public:
    // Unary expression constructor.
    explicit JSUnaryExpr(JSToken unaryOp, Expr* right)
        : unaryOp(std::move(unaryOp)), right(right) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::UnaryExpr; }

    private:
    // Unary operator.
    JSToken unaryOp;
    // Right handside of the unary expression.
    std::unique_ptr<Expr> right;
};

// Literal expression implementation.
class JSLiteralExpr : public Expr<JSBasicValue> {
    public:
    // Constructor takes a JSBasicValue.
    explicit JSLiteralExpr(const JSBasicValue& value) : value(value) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::LiteralExpr; }

    private:
    JSBasicValue value;
};

/// Variable declaration, in JavaScript a variable declaration creates
/// the variable without an assignment. Variable declarations allocate
/// the variable and associate an identifier to it.
/// Because variables declared without an initializer will end up
/// undefined, we can mix variable statements and declarations in our
/// class by having an optional assignemnt.
/// Since assignments can take both bindings or expressions we need a way
/// to represent this in our initializer field.
/// `var a;` : `a` is undefined.
/// `var a = (5 * 3 / 8) + "bob" : `a` is assigned an expression.
/// `var a = function(a,b) { return a;}` : `a` is a binding to a function.

/// Statement is a base class that defines all possible JavaScript statements.
/// BlockStatement for basic blocks.
/// VariableStatement for variable assigments.
/// EmptyStatement for empty statements.
/// ExpressionStatement?
/// IfStatement for conditionals.
/// BreakableStatement :
///     - IterationStatement
///     - SwitchStatement
/// ContinueStatement for continue statements in loops.
/// BreakStatement for break statements in loops.
/// ReturnStatement for function returns.
/// WithStatement for with declarations.
/// LabelledStatement for labels?
/// ThrowStatement for throwing errors and exceptions.
/// TryStatement for try, catch exception handling.
///
/// Declaration is a base class for possible declarations, since all declarations
/// are statements declarations can also inherit from the statement.
/// ClassDeclaration for classes i.e class Person {}.
/// FunctionDeclaration for functions i.e function add() {};
/// VaribaleDeclaration for variables i.e var a;

} // namespace minijsc

#endif
