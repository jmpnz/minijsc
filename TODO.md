# TODO

[ ] Review JS grammar file, ensure all tokens are handled and treat undefined as a keyword.

[x] Review value ownership and stack unwinding in environment guards
    [ ] Implement copy and move constructors for AST nodes

[ ] Make the parser more robust add error handling for parser errors

[ ] Handle nullptr returns when parsing, add error handling to the parser

[ ] Write an AST optimizer for constant folding

[ ] Use ExprRef/StmtRef pattern to build a flat AST

[ ] CMake release flags :
    * Strip debug info with -s
    * Compile with -O3
    * Review the need for exceptions, can we do better ? (-fno-exceptions)
    * We're not using RTTI so (-fno-rtti)
    * -fno-stack-protector
    * -fomit-frame-pointer
    * Ensure we don't use -ffast-math since we deal with double
    * Review linking configuration

[ ] Don't get bogged down in the details, focus on getting a working REPL/VM.
    * Keep the language semantics close to Lox
    * Skip prototypes, global objects, function expressions

[ ] Check [1] to get an idea for how to do tagged pointers for array objects in JS

[ ] Check WebKit blog (FTL, B3) for JIT architecture and implementation details.

[ ] Check the V8 Blog for references on Ignition and TurboFan designs;

[ ] Rewrite our Parser with a focus on performance (Pratt + Flat AST)

[ ] Encoding Numbers : int32, in64 as tagged doubles (high bit sets the tag)

[ ] NaN Boxing (NaN Tagging for all values)

[ ] Check YJIT IR

[ ] Build something like DynASM ?

[ ] Use Octane2 for performance benchmarks

[1]: https://rust-hosted-langs.github.io/book/chapter-interp-tagged-ptrs.html
