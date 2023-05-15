//===----------------------------------------------------------------------===//
// BytecodeCompiler.h: Declaration and implementation of minijsc bytecode
// compiler.
//===----------------------------------------------------------------------===//
#ifndef BYTECODE_COMPILER_H
#define BYTECODE_COMPILER_H

#include "AST.h"
#include "Bytecode.h"

#include <cstdint>

namespace minijsc {

/// Bytecode compiler implements the visitor pattern emitting bytecode
/// at each node.
class BytecodeCompiler : public ASTVisitor {
    public:
    /// Default constructor.
    explicit BytecodeCompiler() = default;
    /// Default destructor.
    ~BytecodeCompiler() override = default;

    /// Return the generated code.
    auto getBytecode() -> std::vector<OPCode> { return bytecodeBuffer; }

    /// Return the constants pool.
    auto getConstantsPool() -> std::vector<JSBasicValue> {
        return constantsPool;
    }

    /// Emit an instruction and write it to the bytecode buffer.
    auto emit(OPCode instruction) -> void {
        bytecodeBuffer.emplace_back(instruction);
    }

    /// Emit an instruction with an immediate value to the bytecode buffer.
    auto emit(OPCode instruction, const JSBasicValue& value) -> void {
        auto offset = addValue(value);
        bytecodeBuffer.emplace_back(instruction);
        bytecodeBuffer.emplace_back((OPCode)offset);
    }

    /// Append a constant to the constant pool returning its index.
    auto addValue(JSBasicValue value) -> size_t {
        constantsPool.emplace_back(value);
        return ((size_t)constantsPool.size() - 1);
    }

    /// Compile an expression.
    auto compile(JSExpr* expr) -> void {
        if (expr != nullptr) {
            fmt::print("compiling an expression of kind {}\n",
                       astNodeKindToString(expr->getKind()));
            expr->accept(this);
        }
    }

    /// Visit a literal expression.
    auto visitLiteralExpr(JSLiteralExpr* expr) -> void override;
    /// Visit a binary expression.
    auto visitBinaryExpr(JSBinExpr* expr) -> void override;
    /// Visit a unary expression.
    auto visitUnaryExpr(JSUnaryExpr* expr) -> void override;
    /// Visit a logical expression.
    auto visitLogicalExpr(JSLogicalExpr* expr) -> void override;
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

    private:
    std::vector<OPCode> bytecodeBuffer;
    std::vector<JSBasicValue> constantsPool;
};

} // namespace minijsc

#endif
