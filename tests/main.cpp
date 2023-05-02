#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/printf.h"

#include "JSLexer.h"
#include "JSToken.h"
#include "JSValue.h"

#include "Bytecode.h"
#include "VM.h"

#include <cstdio>

#include <algorithm>
#include <optional>
#include <string>
#include <variant>
#include <vector>

using namespace minijsc;

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
        auto iter = jsKeywords.find(keyword);
        INFO("Checking keyword: ", keyword);
        CHECK(iter != jsKeywords.end());
    }
}

TEST_CASE("testing the lexing of single character tokens") {
    auto source = "(){}[] a == b\0";
    auto lexer  = JSLexer(source);
    auto tokens = lexer.scanTokens();

    std::vector<JSTokenKind> expected = {
        JSTokenKind::LParen,     JSTokenKind::RParen,
        JSTokenKind::LBrace,     JSTokenKind::RBrace,
        JSTokenKind::LBracket,   JSTokenKind::RBracket,
        JSTokenKind::Identifier, JSTokenKind::EqualEqual,
        JSTokenKind::Identifier, JSTokenKind::Eof,
    };

    for (size_t i = 0; i < tokens.size(); i++) {
        CHECK(tokens[i].getKind() == expected[i]);
    }
}

TEST_CASE("testing the lexing of multicharacter tokens") {
    SUBCASE("var a = 3.14;") {
        auto source = "var a = 3.14;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Var,     JSTokenKind::Identifier, JSTokenKind::Equal,
            JSTokenKind::Numeric, JSTokenKind::Semicolon,  JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("let a = 3.14;") {
        auto source = "let a = 3.14;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Let,     JSTokenKind::Identifier, JSTokenKind::Equal,
            JSTokenKind::Numeric, JSTokenKind::Semicolon,  JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
            if (tokens[i].getLiteral().isNumber()) {
                CHECK(
                    tokens[i].getLiteral().getValue<JSBasicValue::JSNumber>() ==
                    3.14);
            }
        }
    }
    SUBCASE("const a = 3.14;") {
        auto source = "const a = 3.14;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Const,   JSTokenKind::Identifier, JSTokenKind::Equal,
            JSTokenKind::Numeric, JSTokenKind::Semicolon,  JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("var str = \"hello\";") {
        auto source = "var str = \"hello\";";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Var,    JSTokenKind::Identifier, JSTokenKind::Equal,
            JSTokenKind::String, JSTokenKind::Semicolon,  JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("const a = 3.14 >= 7.20;") {
        auto source = "const a = 3.14 >= 7.20;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Const,        JSTokenKind::Identifier,
            JSTokenKind::Equal,        JSTokenKind::Numeric,
            JSTokenKind::GreaterEqual, JSTokenKind::Numeric,
            JSTokenKind::Semicolon,    JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
}

TEST_CASE("testing the lexing of statements and expressions") {
    SUBCASE("statement to assign an expression to a variable") {
        auto source = "var a = 3.14 + 7.86;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Var,       JSTokenKind::Identifier,
            JSTokenKind::Equal,     JSTokenKind::Numeric,
            JSTokenKind::Plus,      JSTokenKind::Numeric,
            JSTokenKind::Semicolon, JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
    SUBCASE("statement to declare a function to add two numbers") {
        auto source = "let adder = function(a,b) { return a + b};";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Let,    JSTokenKind::Identifier,
            JSTokenKind::Equal,  JSTokenKind::Function,
            JSTokenKind::LParen, JSTokenKind::Identifier,
            JSTokenKind::Comma,  JSTokenKind::Identifier,
            JSTokenKind::RParen, JSTokenKind::LBrace,
            JSTokenKind::Return, JSTokenKind::Identifier,
            JSTokenKind::Plus,   JSTokenKind::Identifier,
            JSTokenKind::RBrace, JSTokenKind::Semicolon,
            JSTokenKind::Eof,
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
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Let,        JSTokenKind::Identifier,
            JSTokenKind::Equal,      JSTokenKind::Function,
            JSTokenKind::LParen,     JSTokenKind::Identifier,
            JSTokenKind::RParen,     JSTokenKind::LBrace,
            JSTokenKind::Let,        JSTokenKind::Identifier,
            JSTokenKind::Equal,      JSTokenKind::Numeric,
            JSTokenKind::Semicolon,  JSTokenKind::For,
            JSTokenKind::LParen,     JSTokenKind::Identifier,
            JSTokenKind::Equal,      JSTokenKind::Numeric,
            JSTokenKind::Semicolon,  JSTokenKind::Identifier,
            JSTokenKind::Less,       JSTokenKind::Identifier,
            JSTokenKind::Dot,        JSTokenKind::Identifier,
            JSTokenKind::Semicolon,  JSTokenKind::Identifier,
            JSTokenKind::Plus,       JSTokenKind::Plus,
            JSTokenKind::RParen,     JSTokenKind::LBrace,
            JSTokenKind::Identifier, JSTokenKind::Plus,
            JSTokenKind::Equal,      JSTokenKind::Identifier,
            JSTokenKind::LBracket,   JSTokenKind::Identifier,
            JSTokenKind::RBracket,   JSTokenKind::Semicolon,
            JSTokenKind::RBrace,     JSTokenKind::Return,
            JSTokenKind::Identifier, JSTokenKind::Semicolon,
            JSTokenKind::Eof,
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
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Let,        JSTokenKind::Identifier,
            JSTokenKind::Equal,      JSTokenKind::Function,
            JSTokenKind::LParen,     JSTokenKind::Identifier,
            JSTokenKind::RParen,     JSTokenKind::LBrace,
            JSTokenKind::Let,        JSTokenKind::Identifier,
            JSTokenKind::Equal,      JSTokenKind::Numeric,
            JSTokenKind::Semicolon,  JSTokenKind::While,
            JSTokenKind::LParen,     JSTokenKind::Identifier,
            JSTokenKind::Less,       JSTokenKind::Identifier,
            JSTokenKind::Dot,        JSTokenKind::Identifier,
            JSTokenKind::RParen,     JSTokenKind::LBrace,
            JSTokenKind::Identifier, JSTokenKind::Plus,
            JSTokenKind::Equal,      JSTokenKind::Identifier,
            JSTokenKind::LBracket,   JSTokenKind::Identifier,
            JSTokenKind::RBracket,   JSTokenKind::Semicolon,
            JSTokenKind::RBrace,     JSTokenKind::Return,
            JSTokenKind::Identifier, JSTokenKind::Semicolon,
            JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
    }
}

TEST_CASE("testing the JSBasicValue class") {
    JSBasicValue var(3.14);

    CHECK(var.isNumber() == true);
    CHECK(var.getValue<double>() == 3.14);

    var.setValue(std::string("hulla hoop"));
    CHECK(var.isString() == true);
    CHECK(var.getValue<std::string>() == "hulla hoop");

    JSBasicValue undefined;
    CHECK(undefined.isUndefined() == true);
}

TEST_CASE("testing bytecode virtual machine") {
#define DEBUG_TRACE_EXECUTION
    std::vector<OPCode> bc = {
        OPCode::OPConstant,
        OPCode(0),
        OPCode::OPReturn,
    };
    auto vm = VM(bc);
    vm.storeConstant(JSBasicValue(3.14));

    vm.run();
}

TEST_CASE("testing the Disassembler class") {
    SUBCASE("testing with valid disassembly") {
        std::vector<OPCode> bc = {
            OPCode::OPReturn,
        };

        Disassembler disas(bc, "test-valid-program");
        disas.disassemble();
    }
    SUBCASE("testing with invalid disassembly") {
        std::vector<OPCode> bc = {(OPCode)19, (OPCode)201};

        Disassembler disas(bc, "test-invalid-program");
        disas.disassemble();
    }
}
