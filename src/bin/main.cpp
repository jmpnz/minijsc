#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>

#include "Bytecode.h"
#include "Interpreter.h"
#include "JSLexer.h"
#include "JSParser.h"
#include "JSToken.h"
using namespace minijsc;

/// Read a JavaScript file into an std::string.
auto readJSFile(const std::string& filePath) -> std::string {
    std::string buffer;
    std::ifstream file(filePath);

    file.seekg(0, std::ios::end);
    buffer.resize(file.tellg());
    file.seekg(0);

    file.read(buffer.data(), buffer.size()); // NOLINT

    return buffer;
}

auto main(int argc, char** argv) -> int {
    auto source      = "var a = 42;\n{var a = 39;\n var b = 24;}";
    auto lexer       = JSLexer(source);
    auto tokens      = lexer.scanTokens();
    auto parser      = JSParser(std::move(tokens));
    auto stmts       = parser.parse();
    auto interpreter = Interpreter();
    interpreter.run(stmts);
}
