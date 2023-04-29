#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "Lexer.h"
#include "Token.h"
#include "doctest.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

TEST_CASE("check that all JavaScript keywords are handled") {
    std::vector<std::string> keywords{
        "break", "case", "catch", "class", "const", "continue",
        // "debugger",
        "default", "delete", "do", "else", "export", "extends", "false",
        // "finally",
        "for", "function", "if", "import", "in", "instanceOf", "let", "new",
        "null", "return", "super", "switch", "this", "throw", "true", "try",
        "typeOf", "var",
        // "void",
        "while",
        // "with",
    };

    for (const auto& keyword : keywords) {
        auto iter = minijsc::Keywords.find(keyword);
        INFO("Checking keyword: ", keyword);
        CHECK(iter != minijsc::Keywords.end());
    }
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

TEST_CASE("testing the lexing of statements and expressions") {
    SUBCASE("statement to assign an expression to a variable") {
        auto source = "var a = 3.14 + 7.86;";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Var,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,     minijsc::TokenType::Numeric,
            minijsc::TokenType::Plus,      minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon, minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("statement to declare a function to add two numbers") {
        auto source = "let adder = function(a,b) { return a + b};";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Let,    minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,  minijsc::TokenType::Function,
            minijsc::TokenType::LParen, minijsc::TokenType::Identifier,
            minijsc::TokenType::Comma,  minijsc::TokenType::Identifier,
            minijsc::TokenType::RParen, minijsc::TokenType::LBrace,
            minijsc::TokenType::Return, minijsc::TokenType::Identifier,
            minijsc::TokenType::Plus,   minijsc::TokenType::Identifier,
            minijsc::TokenType::RBrace, minijsc::TokenType::Semicolon,
            minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("statement to declare a function to sum elements of an array") {
        auto source = R"(
        let sumArray = function(arr) {
            let sum = 0;
            for (i = 0;i < arr.len;i++) {
                sum += arr[i];
            }
            return sum;
        )";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Let,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,      minijsc::TokenType::Function,
            minijsc::TokenType::LParen,     minijsc::TokenType::Identifier,
            minijsc::TokenType::RParen,     minijsc::TokenType::LBrace,
            minijsc::TokenType::Let,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,      minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon,  minijsc::TokenType::For,
            minijsc::TokenType::LParen,     minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,      minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon,  minijsc::TokenType::Identifier,
            minijsc::TokenType::Less,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Dot,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Semicolon,  minijsc::TokenType::Identifier,
            minijsc::TokenType::Plus,       minijsc::TokenType::Plus,
            minijsc::TokenType::RParen,     minijsc::TokenType::LBrace,
            minijsc::TokenType::Identifier, minijsc::TokenType::Plus,
            minijsc::TokenType::Equal,      minijsc::TokenType::Identifier,
            minijsc::TokenType::LBracket,   minijsc::TokenType::Identifier,
            minijsc::TokenType::RBracket,   minijsc::TokenType::Semicolon,
            minijsc::TokenType::RBrace,     minijsc::TokenType::Return,
            minijsc::TokenType::Identifier, minijsc::TokenType::Semicolon,
            minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
    SUBCASE("statement to declare a function to sum an array (while)") {
        auto source = R"(
        let sumArray = function(arr) {
            let sum = 0;
            while(i < arr.len) {
                sum += arr[i];
            }
            return sum;
        )";
        auto lexer  = minijsc::Lexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenType> expected = {
            minijsc::TokenType::Let,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,      minijsc::TokenType::Function,
            minijsc::TokenType::LParen,     minijsc::TokenType::Identifier,
            minijsc::TokenType::RParen,     minijsc::TokenType::LBrace,
            minijsc::TokenType::Let,        minijsc::TokenType::Identifier,
            minijsc::TokenType::Equal,      minijsc::TokenType::Numeric,
            minijsc::TokenType::Semicolon,  minijsc::TokenType::While,
            minijsc::TokenType::LParen,     minijsc::TokenType::Identifier,
            minijsc::TokenType::Less,       minijsc::TokenType::Identifier,
            minijsc::TokenType::Dot,        minijsc::TokenType::Identifier,
            minijsc::TokenType::RParen,     minijsc::TokenType::LBrace,
            minijsc::TokenType::Identifier, minijsc::TokenType::Plus,
            minijsc::TokenType::Equal,      minijsc::TokenType::Identifier,
            minijsc::TokenType::LBracket,   minijsc::TokenType::Identifier,
            minijsc::TokenType::RBracket,   minijsc::TokenType::Semicolon,
            minijsc::TokenType::RBrace,     minijsc::TokenType::Return,
            minijsc::TokenType::Identifier, minijsc::TokenType::Semicolon,
            minijsc::TokenType::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].GetType() == expected[i]);
        }
    }
}
