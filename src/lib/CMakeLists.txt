set(minijsc_lib_src
    JSLexer.cpp
    JSToken.cpp
    Bytecode.cpp
)

add_library(libminijsc ${minijsc_lib_src})
