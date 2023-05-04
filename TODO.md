# TODO

[ ] Don't get bogged down in the details, focus on getting a working REPL/VM.
    * Keep the language semantics close to Lox
    * Skip protoypes, global objects, function expressions

[ ] Check [1] to get an idea for how to do tagged pointers for array objects in JS

[ ] Check WebKit blog (FTL, B3) for JIT architecture and implementation details.

[ ] Check the V8 Blog for references on Ignition and TurboFan designs;

[ ] Rewrite our Lexer with a focus on performance

[ ] Encoding Numbers : int32, in64 as tagged doubles (high bit sets the tag)

[ ] NaN Boxing (NaN Tagging for all values)

[ ] Check YJIT IR

[ ] Rename project to chronon

[ ] Build something like DynASM ?

[ ] Use Octane2 for performance benchmarks

[1]: https://rust-hosted-langs.github.io/book/chapter-interp-tagged-ptrs.html
