#include <optional>
#include <variant>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "JSLexer.h"
#include "JSValue.h"

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
        "for", "function", "if", "import", "in", "instanceof", "let", "new",
        "null", "return", "super", "switch", "this", "throw", "true", "try",
        "typeof", "var",
        // "void",
        "while",
        // "with",
    };

    for (const auto& keyword : keywords) {
        auto iter = minijsc::jsKeywords.find(keyword);
        INFO("Checking keyword: ", keyword);
        CHECK(iter != minijsc::jsKeywords.end());
    }
}

TEST_CASE("testing the lexing of single character tokens") {
    auto source = "(){}[] a == b\0";
    auto lexer  = minijsc::JSLexer(source);
    auto tokens = lexer.scanTokens();

    std::vector<minijsc::TokenKind> expected = {
        minijsc::TokenKind::LParen,     minijsc::TokenKind::RParen,
        minijsc::TokenKind::LBrace,     minijsc::TokenKind::RBrace,
        minijsc::TokenKind::LBracket,   minijsc::TokenKind::RBracket,
        minijsc::TokenKind::Identifier, minijsc::TokenKind::EqualEqual,
        minijsc::TokenKind::Identifier, minijsc::TokenKind::Eof,
    };

    for (size_t i = 0; i < tokens.size(); i++) {
        CHECK(tokens[i].getKind() == expected[i]);
    }
}

TEST_CASE("testing the lexing of multicharacter tokens") {
    SUBCASE("var a = 3.14;") {
        auto source = "var a = 3.14;";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Var,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,     minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon, minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("let a = 3.14;") {
        auto source = "let a = 3.14;";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Let,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,     minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon, minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("const a = 3.14;") {
        auto source = "const a = 3.14;";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Const,     minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,     minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon, minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("var str = \"hello\";") {
        auto source = "var str = \"hello\";";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Var,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,     minijsc::TokenKind::String,
            minijsc::TokenKind::Semicolon, minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("const a = 3.14 >= 7.20;") {
        auto source = "const a = 3.14 >= 7.20;";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Const,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,        minijsc::TokenKind::Numeric,
            minijsc::TokenKind::GreaterEqual, minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon,    minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
}

TEST_CASE("testing the lexing of statements and expressions") {
    SUBCASE("statement to assign an expression to a variable") {
        auto source = "var a = 3.14 + 7.86;";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Var,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,     minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Plus,      minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon, minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("statement to declare a function to add two numbers") {
        auto source = "let adder = function(a,b) { return a + b};";
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Let,    minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,  minijsc::TokenKind::Function,
            minijsc::TokenKind::LParen, minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Comma,  minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RParen, minijsc::TokenKind::LBrace,
            minijsc::TokenKind::Return, minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Plus,   minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RBrace, minijsc::TokenKind::Semicolon,
            minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
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
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Let,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Function,
            minijsc::TokenKind::LParen,     minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RParen,     minijsc::TokenKind::LBrace,
            minijsc::TokenKind::Let,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon,  minijsc::TokenKind::For,
            minijsc::TokenKind::LParen,     minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon,  minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Less,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Dot,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Semicolon,  minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Plus,       minijsc::TokenKind::Plus,
            minijsc::TokenKind::RParen,     minijsc::TokenKind::LBrace,
            minijsc::TokenKind::Identifier, minijsc::TokenKind::Plus,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Identifier,
            minijsc::TokenKind::LBracket,   minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RBracket,   minijsc::TokenKind::Semicolon,
            minijsc::TokenKind::RBrace,     minijsc::TokenKind::Return,
            minijsc::TokenKind::Identifier, minijsc::TokenKind::Semicolon,
            minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
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
        auto lexer  = minijsc::JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<minijsc::TokenKind> expected = {
            minijsc::TokenKind::Let,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Function,
            minijsc::TokenKind::LParen,     minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RParen,     minijsc::TokenKind::LBrace,
            minijsc::TokenKind::Let,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Numeric,
            minijsc::TokenKind::Semicolon,  minijsc::TokenKind::While,
            minijsc::TokenKind::LParen,     minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Less,       minijsc::TokenKind::Identifier,
            minijsc::TokenKind::Dot,        minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RParen,     minijsc::TokenKind::LBrace,
            minijsc::TokenKind::Identifier, minijsc::TokenKind::Plus,
            minijsc::TokenKind::Equal,      minijsc::TokenKind::Identifier,
            minijsc::TokenKind::LBracket,   minijsc::TokenKind::Identifier,
            minijsc::TokenKind::RBracket,   minijsc::TokenKind::Semicolon,
            minijsc::TokenKind::RBrace,     minijsc::TokenKind::Return,
            minijsc::TokenKind::Identifier, minijsc::TokenKind::Semicolon,
            minijsc::TokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
}

TEST_CASE("testing the JSBasicValue class") {
    minijsc::JSBasicValue var(3.14);

    CHECK(var.isNumber() == true);
    CHECK(var.getValue<double>() == 3.14);

    var.setValue(std::string("hulla hoop"));
    CHECK(var.isString() == true);
    CHECK(var.getValue<std::string>() == "hulla hoop");

    minijsc::JSBasicValue undefined;
    CHECK(undefined.isUndefined() == true);
}
