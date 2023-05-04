# TODO

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

[ ] Check [1] to get an idea for how to do tagged pointers for array objects in JS

[ ] Check WebKit blog (FTL, B3) for JIT architecture and implementation details.

[ ] Check the V8 Blog for references on Ignition and TurboFan designs;

[ ] Rewrite our Lexer with a focus on performance

[ ] Encoding Numbers : int32, in64 as tagged doubles (high bit sets the tag)

[ ] NaN Boxing (NaN Tagging for all values)

[ ] Check YJIT IR

[ ] Rename project to chronon

[ ] Build something like DynASM ?

[1]: https://rust-hosted-langs.github.io/book/chapter-interp-tagged-ptrs.html
