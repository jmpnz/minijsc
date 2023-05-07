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

class JSExpr;
class JSBinExpr;
class JSLiteralExpr;
class JSUnaryExpr;
class JSGroupingExpr;
class JSVarExpr;
class JSExprStmt;
class JSVarDecl;

/// Visitor interface provides a way to have encapsulate the AST traversal
/// by both the interperter and the bytecode compiler.
struct Visitor {
    virtual ~Visitor() = default;
    // virtual auto visitAssignExpr(Assign expr) -> R     = 0;
    virtual auto visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
        -> JSBasicValue = 0;
    virtual auto visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
        -> JSBasicValue = 0;
    virtual auto visitUnaryExpr(std::shared_ptr<JSUnaryExpr> expr)
        -> JSBasicValue = 0;
    virtual auto visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
        -> JSBasicValue = 0;
    virtual auto visitVarExpr(std::shared_ptr<JSVarExpr> expr)
        -> JSBasicValue = 0;
    // virtual auto visitCallExpr(Call expr) -> R         = 0;
    // virtual auto visitGetExpr(Get expr) -> R           = 0;
    // virtual auto visitLogicalExpr(Logical expr) -> R   = 0;
    // virtual auto visitSetExpr(Set expr) -> R           = 0;
    // virtual auto visitSuperExpr(Super expr) -> R       = 0;
    // virtual auto visitThisExpr(This expr) -> R         = 0;
    // virtual auto visitUnaryExpr(Unary expr) -> R       = 0;
    // virtual auto visitVariableExpr(Variable expr) -> R = 0;
    virtual auto visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void = 0;
    virtual auto visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void   = 0;
};

/// AST node kinds, enumerates both expression and statement ast nodes.
enum class ASTNodeKind {
    // Literal expression.
    LiteralExpr,
    // Binary expression.
    BinaryExpr,
    // Unary expression.
    UnaryExpr,
    // Grouping expression.
    GroupingExpr,
    // Variable expression.
    VarExpr,
    // Variable declaration.
    VarDecl,
    // Expression statement.
    ExprStmt,

};

/// AST node interface encapsulates both expressions and statements.
class ASTNode {
    public:
    ASTNode()          = default;
    virtual ~ASTNode() = default;
};

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
class JSExpr : public ASTNode, public std::enable_shared_from_this<JSExpr> {
    public:
    JSExpr()           = default;
    ~JSExpr() override = default;

    virtual auto getKind() -> ASTNodeKind                 = 0;
    virtual auto accept(Visitor* visitor) -> JSBasicValue = 0;

    // Nested Expr classes here...
};

/// Statement base interface that defines all possible JavaScript statements.
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
class JSStmt : public ASTNode, public std::enable_shared_from_this<JSStmt> {
    public:
    JSStmt()           = default;
    ~JSStmt() override = default;

    virtual auto getKind() -> ASTNodeKind         = 0;
    virtual auto accept(Visitor* visitor) -> void = 0;
};

/// Expression statements.
class JSExprStmt : public JSStmt {
    public:
    // Expression statement constructor takes an expression and associates
    // it to a astatement.
    explicit JSExprStmt(std::shared_ptr<JSExpr> expr) : expr(std::move(expr)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::ExprStmt; }

    // Return the internal expression.
    auto getExpr() -> std::shared_ptr<JSExpr> { return expr; }

    auto accept(Visitor* visitor) -> void override {
        fmt::print("JSExprStmt::accept\n");
        return visitor->visitExprStmt(
            std::static_pointer_cast<JSExprStmt>(shared_from_this()));
    }

    private:
    // Expression associated to the statement.
    std::shared_ptr<JSExpr> expr;
};

/// Variable declarations.
class JSVarDecl : public JSStmt {
    public:
    // Variable statement constructor takes a variable name and an optional
    // initializer.
    explicit JSVarDecl(JSToken name) : name(std::move(name)) {}

    explicit JSVarDecl(JSToken name, std::shared_ptr<JSExpr> expr)
        : name(std::move(name)), initializer(std::move(expr)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::VarDecl; }

    auto accept(Visitor* visitor) -> void override {
        fmt::print("JSVarDecl::accept\n");
        return visitor->visitVarDecl(
            std::static_pointer_cast<JSVarDecl>(shared_from_this()));
    }

    auto getName() -> std::string { return name.getLexeme(); }

    auto getInitializer() -> std::shared_ptr<JSExpr> { return initializer; }

    private:
    // Variable name.
    JSToken name;
    // Initializing expression.
    std::shared_ptr<JSExpr> initializer;
};

/// Variable expression.
class JSVarExpr : public JSExpr {
    public:
    explicit JSVarExpr(JSToken name) : name(std::move(name)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::VarExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSVarExpr::accept\n");
        return visitor->visitVarExpr(
            std::static_pointer_cast<JSVarExpr>(shared_from_this()));
    }

    auto getName() -> JSToken { return name; }

    private:
    JSToken name;
};

/// Binary expression implementation.
class JSBinExpr : public JSExpr {
    public:
    /// Binary expression constructor takes both sides of the expression
    /// and their operand.
    explicit JSBinExpr(std::shared_ptr<JSExpr> left, JSToken binOp,
                       std::shared_ptr<JSExpr> right)
        : left(std::move(left)), right(std::move(right)),
          binOp(std::move(binOp)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::BinaryExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSBinaryExpr::accept\n");
        return visitor->visitBinaryExpr(
            std::static_pointer_cast<JSBinExpr>(shared_from_this()));
    }

    auto getLeft() -> std::shared_ptr<JSExpr> { return left; }

    auto getRight() -> std::shared_ptr<JSExpr> { return right; }

    auto getOperator() -> JSToken { return binOp; }

    private:
    // Left handside of the binary operation.
    std::shared_ptr<JSExpr> left;
    // Right handside of the binary operation.
    std::shared_ptr<JSExpr> right;
    // Binary operator.
    JSToken binOp;
};

// Unary expression implementation.
class JSUnaryExpr : public JSExpr {
    public:
    // Unary expression constructor.
    explicit JSUnaryExpr(JSToken unaryOp, std::shared_ptr<JSExpr> right)
        : unaryOp(std::move(unaryOp)), right(std::move(right)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::UnaryExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSUnaryExpr::accept\n");
        return visitor->visitUnaryExpr(
            std::static_pointer_cast<JSUnaryExpr>(shared_from_this()));
    }

    auto getRight() -> std::shared_ptr<JSExpr> { return right; }

    auto getOperator() -> JSToken { return unaryOp; }

    private:
    // Unary operator.
    JSToken unaryOp;
    // Right handside of the unary expression.
    std::shared_ptr<JSExpr> right;
};

// Literal expression implementation.
class JSLiteralExpr : public JSExpr {
    public:
    // Constructor takes a JSBasicValue.
    explicit JSLiteralExpr(JSBasicValue value)
        : value(std::make_shared<JSBasicValue>(std::move(value))) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::LiteralExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSLiteralExpr::accept\n");
        return visitor->visitLiteralExpr(
            std::static_pointer_cast<JSLiteralExpr>(shared_from_this()));
    }

    auto getValue() -> JSBasicValue { return *value; }

    private:
    std::shared_ptr<JSBasicValue> value;
};

// Grouping expression implementation.
class JSGroupingExpr : public JSExpr {
    public:
    explicit JSGroupingExpr(std::shared_ptr<JSExpr> expression)
        : expr(std::move(expression)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::GroupingExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSGroupingExpr::accept\n");
        return visitor->visitGroupingExpr(
            std::static_pointer_cast<JSGroupingExpr>(shared_from_this()));
    }

    auto getExpr() -> std::shared_ptr<JSExpr> { return expr; }

    private:
    std::shared_ptr<JSExpr> expr;
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

/// Declaration is a base class for possible declarations, since all declarations
/// are statements declarations can also inherit from the statement.
/// ClassDeclaration for classes i.e class Person {}.
/// FunctionDeclaration for functions i.e function add() {};
/// VaribaleDeclaration for variables i.e var a;

} // namespace minijsc

#endif
