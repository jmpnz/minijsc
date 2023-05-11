# TODO

[ ] Make the parser more robust add error handling for parser errors

[ ] Write an AST optimizer for constant folding

[ ] Use ExprRef/StmtRef pattern to build a flat AST

[ ] Don't get bogged down in the details, focus on getting a working REPL/VM.
    * Keep the language semantics close to Lox
    * Skip prototypes, global objects, function expressions

[ ] Check WebKit blog (FTL, B3) for JIT architecture and implementation details.

[ ] Check the V8 Blog for references on Ignition and TurboFan designs;

[ ] Rewrite our Parser with a focus on performance (Pratt + Flat AST)
