#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cstdlib>

#include "JSLexer.h"
#include "JSToken.h"

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
    std::cout << "minijsc : JavaScript compiler suite\n";
    std::cout << "Usage : minijsc <options>\n";
    std::cout << "Run a JavaScript file : minijsc file.js\n";
    std::cout << "Run the REPL : minijsc\n";
    if (argc == 1) {
        std::cout << "Unsupported for now\n.";
        exit(1);
    }
    if (argc == 2) {
        std::string const fileName = argv[1];
        std::string const source   = readJSFile(fileName);
        auto lexer                 = minijsc::JSLexer(source);
        auto tokens                = lexer.scanTokens();

        for (const auto& tok : tokens) {
            std::cout << tok.toString() << ' ';
        }
        std::cout << '\n';
    }
    exit(1);
}
