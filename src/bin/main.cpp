#include <iostream>
#include <string>
#include <vector>

#include "JSLexer.h"
#include "Token.h"

auto main() -> int {
    std::vector<std::string> const msg{
        "Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const std::string& word : msg) {
        std::cout << word << " ";
    }
    std::cout << std::endl;
    const char* source = "let a = 5 * 3;";
    auto tok           = minijsc::Token(minijsc::TokenKind::Let, "LET", "");
    std::cout << tok.toString() << '\n';

    auto lexer = minijsc::JSLexer(source);
    lexer.lex();
    auto tokens = lexer.getTokens();
}
