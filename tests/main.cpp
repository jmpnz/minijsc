#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "Lexer.h"
#include "Token.h"
#include "doctest.h"

#include <cstdio>

TEST_CASE("check tests actually work") {
    auto var = 5;
    CHECK(var == 5);
}

TEST_CASE("testing the lexer") {
    auto source = "(){}[] a == b\0";
    auto lexer  = minijsc::Lexer(source);
    auto tokens = lexer.scanTokens();

    std::vector<minijsc::TokenType> expected = {
        minijsc::TokenType::LParen,     minijsc::TokenType::RParen,
        minijsc::TokenType::LBrace,     minijsc::TokenType::RBrace,
        minijsc::TokenType::LBracket,   minijsc::TokenType::RBracket,
        minijsc::TokenType::Identifier, minijsc::TokenType::EqualEqual,
        minijsc::TokenType::Identifier, minijsc::TokenType::Eof,
    };

    for (size_t i = 0; i < tokens.size(); i++) {
        CHECK(tokens[i].GetType() == expected[i]);
    }
}
