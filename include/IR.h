//===----------------------------------------------------------------------===//
// IR.h: This file implements a lightweight IR for minijsc, the IR is used
// to allow for optimization analysiss that can't be done at the AST level.
//===----------------------------------------------------------------------===//

#ifndef IR_H
#define IR_H

// List of IR instructions, the IR is a stack IR and uses mostly push and pop
// operations.
enum class IRInstruction {
    PUSH,
    POP,
    FADD,
    FSUB,
    FMUL,
    FDIV,
    AND,
    OR,
};

#endif
