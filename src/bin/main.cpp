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

/// Run a given chunk of code.
auto run(std::string source) -> void {
    auto lexer       = JSLexer(source);
    auto tokens      = lexer.scanTokens();
    auto parser      = JSParser(std::move(tokens));
    auto code        = parser.parse();
    auto interpreter = Interpreter();
    interpreter.run(code);
}

/// Run the REPL prompt.
auto runPrompt() -> void {
    std::string source;
    while (true) {
        fmt::print("> ");
        if (std::getline(std::cin, source)) {
            run(source);
        } else {
            fmt::print("\n");
            break;
        }
    }
}

auto main(int argc, char** argv) -> int {
    if (argc > 2) {
        fmt::print("Usage : minijsc [file]\n");
        exit(1);
    } else if (argc == 2) {
        auto filePath = argv[1];
        auto source   = readJSFile(filePath);
        run(source);
    } else {
        runPrompt();
    }
}
