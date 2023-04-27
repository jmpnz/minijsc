#include <iostream>
#include <string>
#include <vector>

#include "Lexer.h"

auto main() -> int {
  std::vector<std::string> const msg{
      "Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

  for (const std::string &word : msg) {
    std::cout << word << " ";
  }
  std::cout << std::endl;
  auto source = "let a = 5 * 3;";
  auto tok = minijsc::Token(minijsc::TokenType::Let, "LET", "");
  std::cout << tok.toString() << '\n';

  auto lexer = minijsc::Lexer(source);
  lexer.lex();
}
