set(minijsc_lib_src
    JSLexer.cpp
    JSToken.cpp
    JSParser.cpp
    Bytecode.cpp
    Interpreter.cpp
    VM.cpp
)

add_library(libminijsc ${minijsc_lib_src})
