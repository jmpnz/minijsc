//===----------------------------------------------------------------------===//
//
// IR.h: This file implements a lightweight IR for minijsc, the IR we call JIR is
// allows us to integrate various compiler optimizations such as constant
// folding, dead code elimination...
//
//===----------------------------------------------------------------------===//

#ifndef IR_H
#define IR_H

/// Simple and Efficient Static Single Assignment Form by Braun et ak.
/// ref: https://pp.info.uni-karlsruhe.de/uploads/publikationen/braun13cc.pdf
///
/// Static Single Assignment Form (SSA) is an IR property where each variable
/// is assigned once.
/// The paper of Braun et al. allows us to turn either the AST or Bytecode
/// into an intermediate representation that is more amenable for optimization
///
/// Local Value Numbering is a process to turn basic blocks into SSA form.
/// e.g :
///
/// let a = 42;        ||    v1 := 42
/// let b = a;         ||    v2 := v1 + v1
/// let c = a + b;     ||    v3 := 23
/// a = c + 23;        ||    v4 := v2 + v3
/// c = a + d;         ||    v5 := v4 + v?
///
/// Since we don't have a definition for "d" it's marked as "v?"

#endif
