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
class JSCallExpr;
class JSVarDecl;
class JSExprStmt;
class JSIfStmt;
class JSBlockStmt;
class JSWhileStmt;
class JSForStmt;
class JSFuncDecl;
class JSReturnStmt;

/// ASTVisitor interface provides a way to have encapsulate the AST traversal
/// by an AST consumer. There are three AST consumers defined currently
/// 1. Optimizer: visits nodes in the tree executing re-writes depending
/// on the optimization selected and node kind.
/// 2. Interpreter: visits nodes in the tree executing statements
/// and evaluating expressions.
/// 3. Bytecode Compiler: visits nodes in the tree emitting bytecode on each
/// node.
/// Since the behavior of each class that traverses the tree will depend
/// on the class itself, we mark all visitor functions as void.
/// This helps us solve a problem with the initial design, interpreter
/// class was the main driver for the Visitor interface so expression
/// visitors returned values but since the optimizer and the compiler don't
/// do any evaluation and either mutate the original expression or emit
/// bytecode the value returned is practically useless.
///
/// Now since the visitors don't return values how will the interpreter
/// do expression evaluation ? Well we can use a value stack, every time
/// we visit an expression we push the value it returns into the value
/// stack and pop it back in the evaluate function after the call to
/// `accept`.
class ASTVisitor {
    public:
    virtual ~ASTVisitor()                                        = default;
    virtual auto visitBinaryExpr(JSBinExpr* expr) -> void        = 0;
    virtual auto visitLiteralExpr(JSLiteralExpr* expr) -> void   = 0;
    virtual auto visitUnaryExpr(JSUnaryExpr* expr) -> void       = 0;
    virtual auto visitGroupingExpr(JSGroupingExpr* expr) -> void = 0;
    virtual auto visitVarExpr(JSVarExpr* expr) -> void           = 0;
    virtual auto visitAssignExpr(JSAssignExpr* expr) -> void     = 0;
    virtual auto visitCallExpr(JSCallExpr* expr) -> void         = 0;
    virtual auto visitReturnStmt(JSReturnStmt* stmt) -> void     = 0;
    virtual auto visitBlockStmt(JSBlockStmt* stmt) -> void       = 0;
    virtual auto visitExprStmt(JSExprStmt* stmt) -> void         = 0;
    virtual auto visitIfStmt(JSIfStmt* stmt) -> void             = 0;
    virtual auto visitWhileStmt(JSWhileStmt* stmt) -> void       = 0;
    virtual auto visitForStmt(JSForStmt* stmt) -> void           = 0;
    virtual auto visitVarDecl(JSVarDecl* stmt) -> void           = 0;
    virtual auto visitFuncDecl(JSFuncDecl* stmt) -> void         = 0;
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
    // Call expressions.
    CallExpr,
    // Variable declarations.
    VarDecl,
    // Block statements.
    BlockStmt,
    // Expression statement.
    ExprStmt,
    // If statement.
    IfStmt,
    // While statement.
    WhileStmt,
    // For statement.
    ForStmt,
    // Return statement.
    ReturnStmt,
    // Function declaration.
    FuncDecl,
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
    case ASTNodeKind::CallExpr:
        return "CallExpr";
    case ASTNodeKind::VarDecl:
        return "VarDecl";
    case ASTNodeKind::BlockStmt:
        return "BlockStmt";
    case ASTNodeKind::ExprStmt:
        return "ExprStmt";
    case ASTNodeKind::IfStmt:
        return "IfStmt";
    case ASTNodeKind::WhileStmt:
        return "WhileStmt";
    case ASTNodeKind::ForStmt:
        return "ForStmt";
    case ASTNodeKind::ReturnStmt:
        return "ReturnStmt";
    case ASTNodeKind::FuncDecl:
        return "FuncDecl";
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

    virtual auto getKind() -> ASTNodeKind            = 0;
    virtual auto accept(ASTVisitor* visitor) -> void = 0;
};

/// Statement base interface that defines all possible JavaScript statements.
class JSStmt : public ASTNode, public std::enable_shared_from_this<JSStmt> {
    public:
    JSStmt()           = default;
    ~JSStmt() override = default;

    virtual auto getKind() -> ASTNodeKind            = 0;
    virtual auto accept(ASTVisitor* visitor) -> void = 0;
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

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSExprStmt::accept\n");
        return visitor->visitExprStmt(static_cast<JSExprStmt*>(this));
    }

    private:
    // Expression associated to the statement.
    std::shared_ptr<JSExpr> expr;
};

/// Return statements return a value to the caller.
class JSReturnStmt : public JSStmt {
    public:
    // Return statement constructor takes the returned value name
    // and the expression to return.
    explicit JSReturnStmt(JSToken keyword, std::shared_ptr<JSExpr> value)
        : keyword(std::move(keyword)), value(std::move(value)) {}

    auto getKeyword() -> JSToken { return keyword; }

    auto getValue() -> std::shared_ptr<JSExpr> { return value; }

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::ReturnStmt; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSReturnStmt::accept\n");
        return visitor->visitReturnStmt(static_cast<JSReturnStmt*>(this));
    }

    private:
    JSToken keyword;
    std::shared_ptr<JSExpr> value;
};

/// Block statements, are blocks of statements to execute. Block statements
/// are enclosed in a scope.
class JSBlockStmt : public JSStmt {
    public:
    // Block statement constructor takes a sequence of statements and associates
    // it to a block.
    explicit JSBlockStmt(std::vector<std::shared_ptr<JSStmt>> stmts)
        : stmts(std::move(stmts)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::BlockStmt; }

    auto getStmts() -> std::vector<std::shared_ptr<JSStmt>> { return stmts; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSBlockStmt::accept\n");
        return visitor->visitBlockStmt(static_cast<JSBlockStmt*>(this));
    }

    private:
    // Statements associated to the block.
    std::vector<std::shared_ptr<JSStmt>> stmts;
};

/// If statements for conditional branching.
class JSIfStmt : public JSStmt {
    public:
    // If statement constructor takes the conditional expression and the branch
    // statements to execute. For `If` statements we only require the block
    // statement for the branch that follows, the else branch will be optional.
    explicit JSIfStmt(std::shared_ptr<JSExpr> expr,
                      std::shared_ptr<JSStmt> thenBranch,
                      std::shared_ptr<JSStmt> elseBranch)
        : condition(std::move(expr)), thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::IfStmt; }

    auto getCondition() -> std::shared_ptr<JSExpr> { return condition; }

    auto getThenBranch() -> std::shared_ptr<JSStmt> { return thenBranch; }

    auto getElseBranch() -> std::shared_ptr<JSStmt> { return elseBranch; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSIfStmt::accept\n");
        return visitor->visitIfStmt(static_cast<JSIfStmt*>(this));
    }

    private:
    // Expression for the conditional branch.
    std::shared_ptr<JSExpr> condition;
    // Then branch statement, executed in case the conditional expression
    // evaluates to true.
    std::shared_ptr<JSStmt> thenBranch;
    // Else branch statement, executed in case the conditional expression
    // evalutes to false.
    std::shared_ptr<JSStmt> elseBranch;
};

/// While statements for looping control flow.
class JSWhileStmt : public JSStmt {
    public:
    // While statement constructor takes the conditional expression and
    // the block to execute if it evaluates to true.
    explicit JSWhileStmt(std::shared_ptr<JSExpr> expr,
                         std::shared_ptr<JSStmt> stmt)
        : condition(std::move(expr)), body(std::move(stmt)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::WhileStmt; }

    auto getCondition() -> std::shared_ptr<JSExpr> { return condition; }

    auto getBody() -> std::shared_ptr<JSStmt> { return body; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSWhileStmt::accept\n");
        return visitor->visitWhileStmt(static_cast<JSWhileStmt*>(this));
    }

    private:
    // Expression for the conditional branch.
    std::shared_ptr<JSExpr> condition;
    // Block statement to execute in case the expression evaluates to true.
    std::shared_ptr<JSStmt> body;
};

// For statements for loops.
class JSForStmt : public JSStmt {
    public:
    // For statement constructor takes the initializer, stopping condition
    // and the step expression. Finally the body of the loop.
    explicit JSForStmt(std::shared_ptr<JSStmt> initializer,
                       std::shared_ptr<JSExpr> condition,
                       std::shared_ptr<JSExpr> step,
                       std::shared_ptr<JSStmt> body)
        : initializer(std::move(initializer)), condition(std::move(condition)),
          step(std::move(step)), body(std::move(body)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::ForStmt; }

    auto getInitializer() -> std::shared_ptr<JSStmt> { return initializer; }

    auto getCondition() -> std::shared_ptr<JSExpr> { return condition; }

    auto getStep() -> std::shared_ptr<JSExpr> { return step; }

    auto getBody() -> std::shared_ptr<JSStmt> { return body; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSForStmt::accept\n");
        return visitor->visitForStmt(static_cast<JSForStmt*>(this));
    }

    private:
    // Expression for the initializer, which can be null.
    std::shared_ptr<JSStmt> initializer;
    // Expression for the stopping condition.
    std::shared_ptr<JSExpr> condition;
    // Expression for the step.
    std::shared_ptr<JSExpr> step;
    // Body of the loop.
    std::shared_ptr<JSStmt> body;
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

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSVarDecl::accept\n");
        return visitor->visitVarDecl(static_cast<JSVarDecl*>(this));
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
    /// Constructor for variable expressions takes the variable name as parameter
    /// since it's the only thing we need to resolve to the variable's value at
    /// runtime.
    explicit JSVarExpr(JSToken name) : name(std::move(name)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::VarExpr; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSVarExpr::accept\n");
        return visitor->visitVarExpr(static_cast<JSVarExpr*>(this));
    }

    auto getName() -> JSToken { return name; }

    private:
    JSToken name;
};

/// Function declarations are statements that create bindings to runtime
/// functions.
class JSFuncDecl : public JSStmt {
    public:
    /// Constructor takes the function name, parameters and statement for the body.
    explicit JSFuncDecl(JSToken name, std::vector<JSToken> params,
                        std::shared_ptr<JSBlockStmt> body)
        : name(std::move(name)), params(std::move(params)),
          body(std::move(body)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::FuncDecl; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSFuncDecl::accept\n");
        return visitor->visitFuncDecl(static_cast<JSFuncDecl*>(this));
    }

    auto getParams() -> std::vector<JSToken> { return params; }

    auto getName() -> JSToken { return name; }

    auto getBody() -> std::shared_ptr<JSBlockStmt> { return body; }

    private:
    /// Function name.
    JSToken name;
    /// Function parameter names.
    std::vector<JSToken> params;
    /// Function body.
    std::shared_ptr<JSBlockStmt> body;
};

/// Assignment expressions, are expressions which after evaluation are bound
/// to a variable.
class JSAssignExpr : public JSExpr {
    public:
    explicit JSAssignExpr(JSToken name, std::shared_ptr<JSExpr> value)
        : name(std::move(name)), value(std::move(value)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::AssignExpr; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSAssignExpr::accept\n");
        return visitor->visitAssignExpr(static_cast<JSAssignExpr*>(this));
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

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSBinaryExpr::accept\n");
        return visitor->visitBinaryExpr(static_cast<JSBinExpr*>(this));
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

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSUnaryExpr::accept\n");
        return visitor->visitUnaryExpr(static_cast<JSUnaryExpr*>(this));
    }

    auto getRight() -> std::shared_ptr<JSExpr> { return right; }

    auto getOperator() -> JSToken { return unaryOp; }

    private:
    // Unary operator.
    JSToken unaryOp;
    // Right handside of the unary expression.
    std::shared_ptr<JSExpr> right;
};

// Call expressions, are expressions that return a value from a function call.
class JSCallExpr : public JSExpr {
    public:
    // Call expression constructor.
    explicit JSCallExpr(std::shared_ptr<JSExpr> callee, JSToken paren,
                        std::vector<std::shared_ptr<JSExpr>> arguments)
        : callee(std::move(callee)), paren(std::move(paren)),
          arguments(std::move(arguments)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::CallExpr; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSCallExpr::accept\n");
        return visitor->visitCallExpr(static_cast<JSCallExpr*>(this));
    }

    auto getCallee() -> std::shared_ptr<JSExpr> { return callee; }

    auto getArgs() -> std::vector<std::shared_ptr<JSExpr>> { return arguments; }

    private:
    // Callee expression.
    std::shared_ptr<JSExpr> callee;
    // Parenthesized token.
    JSToken paren;
    // Arguments list, which is optional.
    std::vector<std::shared_ptr<JSExpr>> arguments;
};

// Literal expressions, are expressions that return a value literal.
class JSLiteralExpr : public JSExpr {
    public:
    // Constructor takes a JSValue.
    explicit JSLiteralExpr(std::shared_ptr<JSValue> value)
        : value(std::move(value)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::LiteralExpr; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSLiteralExpr::accept\n");
        return visitor->visitLiteralExpr(static_cast<JSLiteralExpr*>(this));
    }

    auto getValue() -> std::shared_ptr<JSValue> { return value; }

    private:
    std::shared_ptr<JSValue> value;
};

// Grouping expressions,are expressions enclosed in parentheses overloading
// the default precdence rules.
class JSGroupingExpr : public JSExpr {
    public:
    explicit JSGroupingExpr(std::shared_ptr<JSExpr> expression)
        : expr(std::move(expression)) {}

    auto getKind() -> ASTNodeKind override { return ASTNodeKind::GroupingExpr; }

    auto accept(ASTVisitor* visitor) -> void override {
        fmt::print("JSGroupingExpr::accept\n");
        return visitor->visitGroupingExpr(static_cast<JSGroupingExpr*>(this));
    }

    auto getExpr() -> std::shared_ptr<JSExpr> { return expr; }

    private:
    std::shared_ptr<JSExpr> expr;
};

} // namespace minijsc

#endif
