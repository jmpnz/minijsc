//===----------------------------------------------------------------------===//
//
// This file implements declarations for the abstract syntax tree created
// by the parser. The AST is tree of nodes, the nodes can be declarations,
// statements and expressions.
// To be consistent architecture wise we list a few rules around the nodes
// - Declarations are symbol assignments such as constants, variables
//   or functions.
// - Statements can be declarations, conditionals, loops or function returns.
// - Expressions are produced values.
//===----------------------------------------------------------------------===//
#ifndef AST_H
#define AST_H
#include "JSToken.h"

namespace minijsc {

/// Abstract syntax tree node.
class AstNode {
    public:
    /// Destructor.
    virtual ~AstNode() = default;

    /// Token literal for the node.
    virtual auto tokenLiteral() -> JSToken = 0;
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
