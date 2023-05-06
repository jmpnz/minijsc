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
#include <utility>

#include "JSToken.h"
#include "JSValue.h"

namespace minijsc {

/// Forward declarations for the AST nodes.
class JSExpr;
class JSStmt;
class JSBinExpr;

/// Statement base interface.
class JSStmt {
    public:
    virtual ~JSStmt() = default;
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
class JSExpr {
    public:
    // virtual auto visitBinaryExpr(JSBinExpr expr) -> JSBasicValue = 0;
    virtual ~JSExpr() = default;
};

// Aliases for references to JSExpr and JSStmt implementations.
using JSExprRef = std::unique_ptr<JSExpr>;
using JSStmtRef = std::unique_ptr<JSStmt>;

/// Binary expression implementation.
class JSBinExpr : public JSExpr {
    public:
    /// Binary expression constructor takes both sides of the expression
    /// and their operand.
    explicit JSBinExpr(JSExpr* left, JSToken binOp, JSExpr* right)
        : left(left), right(right), binOp(std::move(binOp)) {}

    private:
    // Left handside of the binary operation.
    JSExprRef left;
    // Right handside of the binary operation.
    JSExprRef right;
    // Binary operator.
    JSToken binOp;
};

// Literal expression implementation.
class JSLiteralExpr : public JSExpr {
    public:
    // Constructor takes a JSBasicValue.
    explicit JSLiteralExpr(const JSBasicValue& value) : value(value) {}

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
class JSVariableDecl {
    public:
    /// Constructor with an identifier token.
    explicit JSVariableDecl(JSToken token) : name(std::move(token)) {}

    private:
    /// Identifier associated to the variable declaration.
    JSToken name;
    /// Optional initializer
    std::optional<JSExprRef> expr;
};

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
