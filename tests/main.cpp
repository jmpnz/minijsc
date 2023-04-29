#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "Lexer.h"
#include "Token.h"
#include "doctest.h"

#include <cstdio>

TEST_CASE("check tests actually work") {
    auto var = 5;
    CHECK(var == 5);
}

TEST_CASE("testing the lexing of single character tokens") {
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

TEST_CASE("testing the lexing of multicharacter tokens") {
    SUBCASE("var a = 3.14;") {
        auto source = "var a = 3.14;";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Var,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,     minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon, minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("let a = 3.14;") {
        auto source = "let a = 3.14;";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Let,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,     minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon, minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("const a = 3.14;") {
        auto source = "const a = 3.14;";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Const,     minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,     minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon, minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("var str = \"hello\";") {
        auto source = "var str = \"hello\";";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Var,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,     minijsc::TokenType::String,
            minijsc::TokenType::Semicolon, minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("const a = 3.14 >= 7.20;") {
        auto source = "const a = 3.14 >= 7.20;";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Const,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,        minijsc::TokenType::Numeric,
            minijsc::TokenType::GreaterEqual, minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon,    minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
}
