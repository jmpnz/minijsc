set(minijsc_lib_src
    ASTOptimizer.cpp
    Bytecode.cpp
    BytecodeCompiler.cpp
    JSLexer.cpp
    JSToken.cpp
    JSParser.cpp
    Interpreter.cpp
    VM.cpp
)

add_library(libminijsc ${minijsc_lib_src})
