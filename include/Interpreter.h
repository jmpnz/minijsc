//===----------------------------------------------------------------------===//
// Interpreter.h: This header defines the class and interface for the baseline
// interpreter for minijsc. The interpreter is a REPL based interpreter with
// an evaluate loop. Evaluation is done at the abstract syntax tree level.
//===----------------------------------------------------------------------===//
#ifndef INTERPRETER_H
#define INTERPRETER_H

#define DEBUG_INTERPRETER_ENV

#include "AST.h"
#include "JSValue.h"
#include "Runtime.h"

#include <memory>
#include <mutex>

namespace minijsc {

/// Interpreter implements runtime evaluation of the abstract syntax tree.
class Interpreter : public Visitor {
    public:
    /// Default constructor.
    explicit Interpreter() : env(std::make_unique<Environment>()) {}

    ~Interpreter() override = default;

    /// Run a sequence of statements (a program).
    auto run(const std::vector<std::shared_ptr<JSStmt>>& stmts) -> void;

    /// Evaluate expression.
    auto evaluate(JSExpr* expr) -> JSBasicValue;
    /// Execute a single statement.
    auto execute(JSStmt* stmt) -> void;
    /// Execute a block (sequence of statements)
    auto executeBlock(JSBlockStmt* block, std::unique_ptr<Environment> env)
        -> void;

    auto visitLiteralExpr(std::shared_ptr<JSLiteralExpr> expr)
        -> JSBasicValue override;
    auto visitBinaryExpr(std::shared_ptr<JSBinExpr> expr)
        -> JSBasicValue override;
    auto visitUnaryExpr(std::shared_ptr<JSUnaryExpr> expr)
        -> JSBasicValue override;
    auto visitGroupingExpr(std::shared_ptr<JSGroupingExpr> expr)
        -> JSBasicValue override;
    auto visitVarExpr(std::shared_ptr<JSVarExpr> expr) -> JSBasicValue override;
    auto visitAssignExpr(std::shared_ptr<JSAssignExpr> expr)
        -> JSBasicValue override;
    auto visitBlockStmt(std::shared_ptr<JSBlockStmt> block) -> void override;
    auto visitExprStmt(std::shared_ptr<JSExprStmt> stmt) -> void override;
    auto visitVarDecl(std::shared_ptr<JSVarDecl> stmt) -> void override;

#ifdef DEBUG_INTERPRETER_ENV

    auto getEnv(const JSToken& name) -> JSBasicValue {
        return env->resolveBinding(name);
    }

#endif

    /// Check truthiness of a value.
    static auto isTruthy(JSBasicValue value) -> bool {
        switch (value.getType()) {
        case JSValueKind::Boolean: {
            return value.getValue<JSBoolean>();
        }
        case JSValueKind::Number: {
            return value.getValue<JSNumber>() != 0 &&
                   value.getValue<JSNumber>() != 0.0;
        }
        case JSValueKind::Undefined:
            return false;
        case JSValueKind::String:
            if (value.getValue<JSString>().empty()) {
                return false;
            }
        case JSValueKind::Null:
            return false;

        default:
            return true;
        }

        return true;
    }

    private:
    /// Runtime environment.
    std::unique_ptr<Environment> env;

    // EnvGuard.
    class EnvGuard {
        public:
        EnvGuard(Interpreter& interpreter, std::unique_ptr<Environment> env)
            : interpreter(interpreter) {
            previous        = std::move(interpreter.env);
            interpreter.env = std::move(env);
        }

        ~EnvGuard() { interpreter.env = std::move(previous); }

        private:
        Interpreter& interpreter;
        std::unique_ptr<Environment> previous;
    };
};

} // namespace minijsc

#endif
