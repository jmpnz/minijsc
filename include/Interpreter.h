//===----------------------------------------------------------------------===//
// Interpreter.h: This header defines the class and interface for the baseline
// interpreter for minijsc. The interpreter is a REPL based interpreter with
// an evaluate loop. Evaluation is done at the abstract syntax tree level.
//===----------------------------------------------------------------------===//
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <ios>
#include <optional>
#define DEBUG_INTERPRETER_ENV

#include "AST.h"
#include "JSRuntime.h"
#include "JSValue.h"

#include <memory>
#include <mutex>
#include <utility>

namespace minijsc {

/// Max possiblee number of scopes.
static constexpr int kMaxNestedScopes = 65535;

/// Interpreter implements runtime evaluation of the abstract syntax tree.
class Interpreter : public ASTVisitor {
    public:
    /// Default constructor.
    explicit Interpreter() {
        // Allocate for enough scopes.
        symTables.reserve(kMaxNestedScopes);
        // Set the index of the current environment to 0.
        currIdx = 0;
        // Append a new environment and set it's parent to -1.
        auto topLevel = Environment();
        topLevel.setParentPtr(-1);
        symTables.emplace_back(topLevel);
    }

    ~Interpreter() override = default;

    /// Return the interpeter's current scope pointer.
    [[nodiscard]] auto getCurrIdx() const -> EnvPtr { return currIdx; }

    /// Set the current scope's index to a new value.
    auto setCurrIdx(EnvPtr idx) -> void { currIdx = idx; }

    // Add a new scope to the global scopes table.
    auto appendSymbolTable(const Environment& env) -> void {
        symTables.emplace_back(env);
        currIdx += 1;
    }

    // Pop a scope from the symbol tables.
    auto popSymbolTable() -> void {
        symTables.pop_back();
        currIdx -= 1;
    }

    /// Define a binding, definitions of new bindings always go into
    /// the current scope.
    auto define(const std::string& name, std::shared_ptr<JSValue> value)
        -> void {
        symTables[currIdx].defineBinding(name, std::move(value));
    }

    /// Assign abinding.
    auto assign(const std::string& name, const std::shared_ptr<JSValue>& value)
        -> void {
        // In order to create an assignment we need to check scopes
        // in the reverse order they were created in.
        // Starting from the current scope and iterating until we reach
        // the global scope at index 0.
        auto idx = currIdx;
        while (idx != -1) {
            // If the variable exists this scope, try and create an assignment.
            if (symTables[idx].resolveBinding(name) != nullptr) {
                if (symTables[idx].assign(name, value)) {
                    // If the assignment is successful return.
                    return;
                }
            }
            // If the variable wasn't found in the current scope, move to its parent.
            idx = symTables[idx].getParentPtr();
        }
        // If the variable isn't found in all the scopes throw a runtime error
        throw std::runtime_error(
            fmt::format("Variable {} is undefined.\n", name));
    }

    // Resolve a binding
    auto resolve(const std::string& name) -> std::shared_ptr<JSValue> {
        // Similar to assignment the runtime starts by checking the current scope
        // if the binding is found we return the value. Otherwise we move to the
        // parent scope.
        auto idx = currIdx;
        while (idx != -1) {
            // If the variable existis within this scope we return it.
            auto value = symTables[idx].resolveBinding(name);
            if (value != nullptr) {
                return value;
            }
            // If the variable wasn't found in the current scope we move to it
            // parent.
            idx = symTables[idx].getParentPtr();
        }
        // If the variable isn't found in the existing scopes we throw a runtime
        // error.
        throw std::runtime_error(
            fmt::format("Variable {} is undefined.\n", name));
    }

    /// Run a sequence of statements (a program).
    auto run(const std::vector<std::shared_ptr<JSStmt>>& stmts) -> void;

    /// Evaluate expression.
    auto evaluate(JSExpr* expr) -> std::shared_ptr<JSValue>;
    /// Execute a single statement.
    auto execute(JSStmt* stmt) -> void;
    /// Execute a block (sequence of statements)
    auto executeBlock(JSBlockStmt* block, Environment env) -> void;

    /// Push an evaluated value into the value stack.
    auto pushValue(std::shared_ptr<JSValue> value) -> void {
        valueStack.emplace_back(value);
    }

    // Pop a value from the value stack.
    auto popValue() -> std::shared_ptr<JSValue> {
        auto value = valueStack.back();
        valueStack.pop_back();
        return value;
    }

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

#ifdef DEBUG_INTERPRETER_ENV

    auto getEnv(const JSToken& name) -> std::shared_ptr<JSValue> {
        auto value = symTables[currIdx].resolveBinding(name.getLexeme());
        if (value != nullptr) {
            return value;
        }

        fmt::print("Value undefined .\n");
        return {nullptr};
    }

    auto getValue(const JSToken& name) -> JSBasicValue {
        auto value = getEnv(name);

        auto result = std::static_pointer_cast<JSBasicValue>(value);
        return *result;
    }

#endif
    /// Check truthiness of a value.
    static auto isTruthy(const std::shared_ptr<JSValue>& value) -> bool {
        auto basicValue = std::static_pointer_cast<JSBasicValue>(value);
        switch (basicValue->getKind()) {
        case JSValueKind::Boolean: {
            return basicValue->getValue<JSBoolean>();
        }
        case JSValueKind::Number: {
            return basicValue->getValue<JSNumber>() != 0 &&
                   basicValue->getValue<JSNumber>() != 0.0;
        }
        case JSValueKind::Undefined:
            return false;
        case JSValueKind::String:
            if (basicValue->getValue<JSString>().empty()) {
                return false;
            }
        case JSValueKind::Null:
            return false;

        default:
            return true;
        }

        return true;
    }

    /// Runtime environment is designed following the same ideas of spaghetti
    /// stack. We have a stack of environments, the stack is always populated
    /// with at least one top level environment (the global environment).
    ///
    /// Upon entering a block statement, we push a new environment to the stack
    /// this environment is for defining bindings in the new inner scope.
    /// Upon exiting a block statement we pop the inner environment from
    /// the stack and restore the runtime's pointer (`currIdx`).
    ///
    /// When assigning a variable you first check if its defined in the global
    /// scope, if it isn't defined we move the outer scope using the parent
    /// pointer and assign it there.
    /// If we reach the top level scope without making an assignment it means
    /// the variable is undefined, the same process is used for resolving
    /// bindings.
    ///
    /// Runtime environment is a stack of symbol tables.
    std::vector<Environment> symTables;
    /// Value stack to keep track of evaluated expressions.
    std::vector<std::shared_ptr<JSValue>> valueStack;
    /// Pointer to the current environment.
    EnvPtr currIdx;

    private:
};

} // namespace minijsc

#endif
