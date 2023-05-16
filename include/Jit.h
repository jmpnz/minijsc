//===----------------------------------------------------------------------===//
// Jit.h: This file contains definitions for the classes and functions for JIT
// compilation.
//
// The JIT compiler we implement is a bytecode level tracing JIT. JIT compilers
// typically operaete at the bytecode level and do several transformations on
// the "hot" bytecode before transpiling it to the host's backend machine code
// in our case we chose aarch64 as a backend.
//
// The JIT we implement here has a very compact structure, composed of a JIT
// cache which keeps a cache of jitted bytecode and a reference to the VM.
//
// The engine functions in two phases, during the first phase the bytecode
// executed by the VM is profiled, the profiling of the entire bytecode results
// in traces. Traces are essentially a datastructure that represents a trace
// of execution of the bytecode recording instruction occurences within blocks
// and functions. When a function is marked as hot we compile the traced code
// to machine code and create a function pointer within the JitContext.
//
// JitContext is mainly used to encapsulate the machine code and makes the heap
// allocations required later for execution. We then write the bytecode to the
// allocated memory page and mark it as executable. This context is wwrapped in
// a function that makes the syscalls to get the permissions to execute JIT
// code.
//
// The first phase sets the stage for the second and last phase, execution.
//
// When the VM enters a hot section again (VM keeps pointers to top-k traces)
// it skips the bytecode and makes a function call to the jitted function
// storing the result on the top of the stack.
//===----------------------------------------------------------------------===//
#ifndef JIT_H
#define JIT_H

struct TraceContext {};

struct JitCache {};

struct JitContext {};

#endif
