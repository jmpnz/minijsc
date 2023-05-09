//===----------------------------------------------------------------------===//
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

/// Forward declarations of abstract classes representing AST nodes.
class JSExpr;
class JSStmt;
/// Forward declarations of concrete classes representing AST nodes.
class JSBinExpr;
class JSLiteralExpr;
class JSUnaryExpr;
class JSGroupingExpr;
class JSVarExpr;
class JSAssignExpr;
class JSVarDecl;
class JSExprStmt;
class JSBlockStmt;

/// Visitor interface provides a way to have encapsulate the AST traversal
/// by an AST consumer. There are three AST consumers defined currently
/// 1. Optimizer: visits nodes in the tree executing re-writes depending
/// on the optimization selected and node kind.
/// 2. Interpreter: visits nodes in the tree executing statements
/// and evaluating expressions.
/// 3. Bytecode Compiler: visits nodes in the tree emitting bytecode on each
/// node.
struct Visitor {
    /// Default destructor, since Visitor doesn't manage the lifecycle of AST
    /// nodes it doesn't require a special destructor.
    virtual ~Visitor() = default;
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
    virtual auto visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
        -> JSBasicValue = 0;
    // virtual auto visitCallExpr(Call expr) -> R         = 0;
    // virtual auto visitGetExpr(Get expr) -> R           = 0;
    // virtual auto visitLogicalExpr(Logical expr) -> R   = 0;
    // virtual auto visitSetExpr(Set expr) -> R           = 0;
    // virtual auto visitSuperExpr(Super expr) -> R       = 0;
    // virtual auto visitThisExpr(This expr) -> R         = 0;
    // virtual auto visitUnaryExpr(Unary expr) -> R       = 0;
    // virtual auto visitVariableExpr(Variable expr) -> R = 0;
    virtual auto visitBlockStmt(std::shared_ptr<JSBlockStmt> stmt) -> void = 0;
    virtual auto visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void   = 0;
    virtual auto visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void     = 0;
};

/// AST node kinds, enumerates both expression and statement nodes.
enum class ASTNodeKind {
    // Literal expressions, such as value literals.
    LiteralExpr,
    // Binary expressions.
    BinaryExpr,
    // Unary expressions.
    UnaryExpr,
    // Grouping expressions, which use parenthesis to overload precedence.
    GroupingExpr,
    // Assignment expressions.
    AssignExpr,
    // Variable expressions.
    VarExpr,
    // Variable declarations.
    VarDecl,
    // Block statements.
    BlockStmt,
    // Expression statement.
    ExprStmt,

};

/// Returns a textual representation of the AST node kind.
inline auto astNodeKindToString(ASTNodeKind kind) -> std::string {
    switch (kind) {
    case ASTNodeKind::LiteralExpr:
        return "LiteralExpr";
    case ASTNodeKind::BinaryExpr:
        return "BinaryExpr";
    case ASTNodeKind::UnaryExpr:
        return "UnaryExpr";
    case ASTNodeKind::GroupingExpr:
        return "GroupingExpr";
    case ASTNodeKind::AssignExpr:
        return "AssignExpr";
    case ASTNodeKind::VarExpr:
        return "VarExpr";
    case ASTNodeKind::VarDecl:
        return "VarDecl";
    case ASTNodeKind::BlockStmt:
        return "BlockStmt";
    case ASTNodeKind::ExprStmt:
        return "ExprStmt";
    }
}

/// AST node interface encapsulates both expressions and statements.
class ASTNode {
    public:
    ASTNode()          = default;
    virtual ~ASTNode() = default;
};

/// Expression base interface, the AST is built and evaluated using the visitor
/// pattern. The base JSExpr type defines all possible JavaScript expressions
/// each consumer of the AST defines the behavior of how a JSExpr is processed.
class JSExpr : public ASTNode, public std::enable_shared_from_this<JSExpr> {
    public:
    JSExpr()           = default;
    ~JSExpr() override = default;

    virtual auto getKind() -> ASTNodeKind                 = 0;
    virtual auto accept(Visitor* visitor) -> JSBasicValue = 0;
};

/// Statement base interface that defines all possible JavaScript statements.
class JSStmt : public ASTNode, public std::enable_shared_from_this<JSStmt> {
    public:
    JSStmt()           = default;
    ~JSStmt() override = default;

    virtual auto getKind() -> ASTNodeKind         = 0;
    virtual auto accept(Visitor* visitor) -> void = 0;
};

/// Expression statements, are statements that invoke or execute expressions.
/// Example of expression statements are function calls, variable assignments
/// or in-place increments.
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

/// Block statements, are blocks of statements to execute. Block statements
/// are enclosed in a scope.
class JSBlockStmt : public JSStmt {
    public:
    // Block statement constructor takes a sequence of statements and associates
    // it to a block.
    explicit JSBlockStmt(std::vector<std::shared_ptr<JSStmt>> stmts)
        : stmts(std::move(stmts)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::BlockStmt; };

    auto getStmts() -> std::vector<std::shared_ptr<JSStmt>> { return stmts; }

    auto accept(Visitor* visitor) -> void override {
        fmt::print("JSBlockStmt::accept\n");
        return visitor->visitBlockStmt(
            std::static_pointer_cast<JSBlockStmt>(shared_from_this()));
    }

    private:
    // Statements associated to the block.
    std::vector<std::shared_ptr<JSStmt>> stmts;
};

/// Variable declarations are statements that create runtime bindings
/// to associate a variable to an expression.
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

/// Variable expressions, are expressions which return the value bound to
/// a variable.
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

/// Assignment expressions, are expressions which after evaluation are bound
/// to a variable.
class JSAssignExpr : public JSExpr {
    public:
    explicit JSAssignExpr(JSToken name, std::shared_ptr<JSExpr> value)
        : name(std::move(name)), value(std::move(value)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::AssignExpr; }

    auto accept(Visitor* visitor) -> JSBasicValue override {
        fmt::print("JSAssignExpr::accept\n");
        return visitor->visitAssignExpr(
            std::static_pointer_cast<JSAssignExpr>(shared_from_this()));
    }

    auto getName() -> JSToken { return name; }

    auto getValue() -> std::shared_ptr<JSExpr> { return value; }

    private:
    JSToken name;
    std::shared_ptr<JSExpr> value;
};

/// Binary expressions, are expressions that encapsulate binary operations.
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

// Unary expressions, are expressions that encapsulate unary operations.
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

// Literal expressions, are expressions that return a value literal.
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

// Grouping expressions,are expressions enclosed in parentheses overloading
// the default precdence rules.
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

} // namespace minijsc

#endif
