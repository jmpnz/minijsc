# Architecture of minijsc

## Runtime Architecture Design :

Currently minijsc absorbs Javascript runs it through the Lexer and outputs
a sequence of tokensi, the parsing engine then consumes the token and builds
an abstract syntax tree, the interpreter is able to eval the tree by running
though the AST and executing expressions using the host language (C++).
If the bytecode VM is used instead then the AST is compiled to Bytecode
that is then executed in the virtual machine (stack based).

This represents the baseline and benchmarks are available through Google
Benchmark library see [benches/interperter.cpp] and [benches/bytecode.cpp].

Our goal now is to measure and establish the baseline for workloads which
can be done in a few steps.

* Define global API for the runtime for each execution engine (interpreter, vm)
* Write integration tests to simulate JavaScript workloads
* Use GBenchmark to setup benchmark suites for each or all workloads
* Execute and collect results then plot in gnuplot

Now that we have a baseline we can start thinking about optimizations, first
we need to establish our bottlenecks, whether we are CPU or memory bound will
determine which places you need to profile.

Architecture wise we should probably use a flat AST style datastructure as
they are fast. For our interpreter we can inline and optimize our execution
functions.

But more importantly there are two features we want to implement :

* Design a register based virtual machine and use it instead of the stack one
* Design an IR that supports SSA form and do run optimization passes
* Use a nanopass based approach to optimize the SSA
* Write a parser that compiles to the IR instead of bytecode directly
* Compile the IR to bytecode
* Bytecode pass, Ignition uses the following pass :

Parser -> AST -> Bytecode -> Register Optimizer -> Peephole Optimizer -> Deadcode Elimination
                                                                              |
                                       Virtual Machine <-  Bytecode Writer  <-+

Finally the most important optimization I can think of is writing a JIT
for our IR to RISC-V and maybe Aarch64.

# Register based virtual machine

* First we define 8 registers : [r0;r7]
* ABI: Function calls pass arguments from register 2 to 7 with r0 being IP register
* Load/Store semantics vs move semantics ?
* E.g add [r0], [r1], r[2]
* <fn add> before passing IP to add's entry point
    * args(<fn add>) are stored in registers r3, r4
    * add r5, r3, r4
    * ret [store result in r7]
    * clean up stack, set IP to stack top, assuming local vars are in the stack
    * current IP points to offset at the top of the stack
    * break execution fetch next IP

# Bytecode Virtual Machine

```cpp

class VMContext {
    std::vector<JSNumericValue> constantsPool;
};


class VM {
    VMContext ctx,
};
```
`OP_CNST` [0xa, 0xff, 0xff, 0xff]
 1 byte followed by 3 byte index in the constant pool ?

minijsc can be described by the following graph, initially JavaScript source
code is scanned and processed into tokens. The parser will then process tokens
to build an abstract syntax tree. At this point you can execute the code using
various implementation paradigms

* Interpreter backed by a register based virtual machine
* JIT Compile the bytecode to RISC-V


```sh
+------------------------+
| JavaScript Source Code |
+------------------------+
           |
           |
           v
+-------------------------+
|  Abstract Syntax Tree   |
+-------------------------+
           |
           |
           v
+-------------------------+
|      AST Optimizer      |
+-------------------------+
           |
           |
           v
+-------------------------+
| Intermediate            |
| Representation (IR)     |
+-------------------------+
           |
           |
           v
+-------------------------+
|       Optimized IR      |
+-------------------------+
           |
           |
           v
+-------------------------+
| Machine Code Generation |
+-------------------------+
           |
           |
           v
+-------------------------+
| Executable Machine Code |
+-------------------------+
```

## Runtime

The minijsc runtime is based around the idea of an execution context.
An execution contexts is an environment that hold scope bindings and
resolvers for values within an execution scope.

Contexts are used pretty much everywhere, when you start executing
code in the minijsc virtual machine a global context is created
and exposes the JavaScript runtime API, which exposes thinks like
`Uint8Array`, `ArrayBuffer`...

The second context is created at the module level, per file, each
module specific context will load all imported files and expose
the exported units in that file (types, functions...) in its context.

## JSValues and JSObjects

This was largely inspired by the design of [V8] although because we
are pretty slim feature wise we omit several "features" of the core
V8 implementation.
