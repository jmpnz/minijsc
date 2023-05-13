#include "AST.h"
#include "ASTOptimizer.h"
#include "BytecodeCompiler.h"
#include "Interpreter.h"
#include "JSParser.h"
#include <memory>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "fmt/printf.h"

#include "JSLexer.h"
#include "JSParser.h"
#include "JSToken.h"
#include "JSValue.h"

#include "Bytecode.h"
#include "VM.h"

#define DEBUG_INTERPRETER_ENV

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
                CHECK(tokens[i].getLiteral().getValue<JSNumber>() == 3.14);
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

TEST_CASE("testing the parser") {
    SUBCASE("testing the parser match") {
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
        auto parser = JSParser(tokens);
        CHECK(parser.match({JSTokenKind::Let, JSTokenKind::Identifier}) ==
              true);
        CHECK(parser.match({JSTokenKind::Function, JSTokenKind::Var}) == false);
    }
    SUBCASE("test parsing expressions/literals(true)") {
        auto source = "true;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(tokens);
        auto expr   = parser.parsePrimaryExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::LiteralExpr);
    }
    SUBCASE("test parsing expressions/comparison(not equal)") {
        auto source = "1 != 2;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test parsing expressions/comparison(equal)") {
        auto source = "2 == 2;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test parsing expressions/term(plus)") {
        auto source = "1 + 2;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test parsing expressions/factor(star)") {
        auto source = "1 + 2 * 3;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test parsing expressions/grouping(comparison)") {
        auto source = "(4 == 2);";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::GroupingExpr);
    }
    SUBCASE("test parsing expressions/comparison(grouping)") {
        auto source = "3 == (1 + 2);";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();

        std::vector<JSTokenKind> expected = {
            JSTokenKind::Numeric, JSTokenKind::EqualEqual, JSTokenKind::LParen,
            JSTokenKind::Numeric, JSTokenKind::Plus,       JSTokenKind::Numeric,
            JSTokenKind::RParen,  JSTokenKind::Semicolon,  JSTokenKind::Eof,
        };

        for (size_t i = 0; i < tokens.size(); i++) {
            CHECK(tokens[i].getKind() == expected[i]);
        }
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
}

TEST_CASE("testing interpreter evaluate") {
    SUBCASE("test interpreting boolean literal(true)") {
        auto source      = "true;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value = (JSBasicValue*)interpreter.evaluate(expr.get()).get();
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == JSBoolean(true));
        CHECK(expr.get()->getKind() == ASTNodeKind::LiteralExpr);
    }
    SUBCASE("test interpreting boolean literal(false)") {
        auto source      = "false;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value = (JSBasicValue*)interpreter.evaluate(expr.get()).get();
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == JSBoolean(false));
        CHECK(expr.get()->getKind() == ASTNodeKind::LiteralExpr);
    }
    SUBCASE("test interpreting unary expressions (-1)") {
        auto source      = "-1;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        fmt::print("test/ {}\n", value->toString());
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(-1));
        CHECK(expr.get()->getKind() == ASTNodeKind::UnaryExpr);
    }
    SUBCASE("test interpreting unary expressions (truthy/undefined)") {
        auto source      = "!undefined;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == JSBoolean(true));
        CHECK(expr.get()->getKind() == ASTNodeKind::UnaryExpr);
    }
    SUBCASE("test interpreting unary expressions (truthy/null)") {
        auto source      = "!null;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == JSBoolean(true));
        CHECK(expr.get()->getKind() == ASTNodeKind::UnaryExpr);
    }
    SUBCASE("test interpreting binary expressions (add)") {
        auto source      = "1 + 3;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(4));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting grouped expressions (add/mul)") {
        auto source      = "(1 + 3) * 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(20));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting grouped expressions (mul/add)") {
        auto source      = "(3 * 5) + 1;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(16));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting unary expressions (!false)") {
        auto source      = "!false;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == JSBoolean(true));
        CHECK(expr.get()->getKind() == ASTNodeKind::UnaryExpr);
    }
    SUBCASE("test interpreting binary expressions (add)") {
        auto source      = "1 + 3;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(4));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting grouped expressions (add/mul)") {
        auto source      = "(1 + 3) * 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(20));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting grouped expressions (mul/add)") {
        auto source      = "(3 * 5) + 1;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(16));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting grouped expressions (add/mul) no parenthesis") {
        auto source      = "1 + 3 * 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isNumber() == true);
        CHECK(value->getValue<JSNumber>() == JSNumber(16));
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/greater_equal)") {
        auto source      = "5 >= 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == true);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/greater)") {
        auto source      = "5 > 4;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == true);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/lesser_equal)") {
        auto source      = "4 <= 4;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == true);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/lesser)") {
        auto source      = "3 < 4;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == true);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/unequal") {
        auto source      = "3 != 4;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == true);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }
    SUBCASE("test interpreting binary expressions (comparison/equal_equal)") {
        auto source      = "3 == 4;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto expr        = parser.parseExpr();
        auto interpreter = Interpreter();
        auto value       = std::static_pointer_cast<JSBasicValue>(
            interpreter.evaluate(expr.get()));
        CHECK(value->isBoolean() == true);
        CHECK(value->getValue<JSBoolean>() == false);
        CHECK(expr.get()->getKind() == ASTNodeKind::BinaryExpr);
    }

    SUBCASE("test interpreting variable declaration") {
        auto source      = "var a = 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmt        = parser.parseDecl();
        auto interpreter = Interpreter();
        interpreter.execute(stmt.get());
        auto result = std::static_pointer_cast<JSBasicValue>(
            interpreter.getEnv(JSToken(JSTokenKind::Identifier, "a", 0.)));
        REQUIRE(result != nullptr);
        CHECK((*result).getValue<JSNumber>() == 5.);
    }
    SUBCASE("test interpreting variable declaration/let") {
        auto source      = "let a = 5;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmt        = parser.parseDecl();
        auto interpreter = Interpreter();
        interpreter.execute(stmt.get());
        auto result = std::static_pointer_cast<JSBasicValue>(
            interpreter.getEnv(JSToken(JSTokenKind::Identifier, "a", 0.)));
        REQUIRE(result != nullptr);
        CHECK((*result).getValue<JSNumber>() == 5.);
    }
    SUBCASE("test interpreting variable declarations with binary expression") {
        auto source      = "var a = 5;\nvar b = 37;\nvar c = a + b;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        interpreter.run(stmts);
        auto result = std::static_pointer_cast<JSBasicValue>(
            interpreter.getEnv(JSToken(JSTokenKind::Identifier, "c", 0.)));
        REQUIRE(result != nullptr);
        CHECK((*result).getValue<JSNumber>() == 42.);
    }
    SUBCASE("test interpreting variable assignment") {
        auto source      = "var a = 42;\n a = 39;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        interpreter.run(stmts);
        auto result = std::static_pointer_cast<JSBasicValue>(
            interpreter.getEnv(JSToken(JSTokenKind::Identifier, "a", 0.)));
        REQUIRE(result != nullptr);
        CHECK((*result).getValue<JSNumber>() == 39.);
    }
    SUBCASE("test interpreting block statements") {
        auto source      = "var a = 42;\n{var a = 39;\n var b = 24;}";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        interpreter.run(stmts);
        fmt::print("Run done\n");
    }
    SUBCASE("test interpreting variable assignment with binary expressions") {
        auto source =
            "var a = 42;\n a = 37;\nvar b = 5;\nvar c = a + b;\n var "
            "d = !true;\n var f = d == false;\na = -55;\n var g = a + c;";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        interpreter.run(stmts);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "a", 0.))
                  .getValue<JSNumber>() == -55.);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "c", 0.))
                  .getValue<JSNumber>() == 42.);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "d", 0.))
                  .getValue<JSBoolean>() == false);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "f", 0.))
                  .getValue<JSBoolean>() == true);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "g", 0.))
                  .getValue<JSNumber>() == -13.);
    }
    SUBCASE("test interpreting expression (plus operator overload)") {
        auto source      = "var a = \"hello\";\nvar b = false;\n var c = "
                           "true;\n var d = 72;\n var e = 28;\nvar f = a + "
                           "\"Bob\";\nvar g = a + b;\nvar h = a + c;\n";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        interpreter.run(stmts);
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "f", 0.))
                  .getValue<JSString>() == "helloBob");
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "g", 0.))
                  .getValue<JSString>() == "hellofalse");
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "h", 0.))
                  .getValue<JSString>() == "hellotrue");
    }
    SUBCASE("test interpreting expression with outer and inner scope") {
        auto source      = "var a = 1;\n{var a = a + 2;\n}";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "a", 0.))
                  .getValue<JSNumber>() == 1.);
    }
    SUBCASE(
        "test interpreting expression with outer and multiple inner scopes") {
        auto source      = "var a = 1;\n{var a = a + 2;\n{var b = a + 3;}\n}";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "a", 0.))
                  .getValue<JSNumber>() == 1.);
    }
    SUBCASE("test interpreting conditional expressions") {
        auto source      = "var a = 1;\nif (true){a = 2;}";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "a", 0.))
                  .getValue<JSNumber>() == 2.);
    }
    SUBCASE("test interpreting conditional expressions with else branch") {
        auto source      = "var a = 1;\nif (false){a = 2;} else { a = 3; }";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "a", 0.))
                  .getValue<JSNumber>() == 3.);
    }
    SUBCASE("test interpreting while loop") {
        auto source = "var i = 0;\nvar sum = 0;\nwhile ( i < 10) { sum = sum + "
                      "1; i = i + 1; }";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto stmts  = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "sum", 0.))
                  .getValue<JSNumber>() == 10.);
    }
    SUBCASE("test interpreting for loop with variable declaration") {
        auto source = "var sum = 0;\nfor (var i = 0;i < 10;i = i + 1) { sum = "
                      "sum + 1; }\n";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto stmts  = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "sum", 0.))
                  .getValue<JSNumber>() == 10.);
    }
    SUBCASE("test interpreting for loop with pre-variable declaration") {
        auto source =
            "var sum = 0;\nvar i = -1;\nfor (i = 0;i < 10;i = i + 1) { sum = "
            "sum + 1; }\n";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "sum", 0.))
                  .getValue<JSNumber>() == 10.);
    }
    SUBCASE("test interpreting for loop with in-loop step") {
        auto source      = "var sum = 0;\nvar i = 0;\nfor (;i < 10;) { sum = "
                           "sum + 1;i = i +1; }\n";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "sum", 0.))
                  .getValue<JSNumber>() == 10.);
    }
    SUBCASE("test interpreting function calls") {
        auto source      = "function add(a, b) {var d = a + b;}\n add(1,2);";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
    }
    SUBCASE("test interpreting function calls with return values") {
        auto source =
            "function add(a, b) {var sum = a + b; return sum;}\nvar d "
            "= add(1,2);";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "d", 0.))
                  .getValue<JSNumber>() == 3.);
    }
    /*
    SUBCASE("test interpreting function calls with return values/fib(5)") {
        auto source      = "function fib(n) { if (n <= 1) { return n; } return "
                           "fib(n - 2) + fib(n - 1);} var x = fib(5);";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "x", 0.))
                  .getValue<JSNumber>() == 5.);
    }
    SUBCASE("test interpreting recursive function calls/factorial(5)") {
        auto source = "function factorial(x) { return x * factorial(x-1);}\n "
                      "var res = factorial(5);";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto stmts  = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(interpreter.getValue(JSToken(JSTokenKind::Identifier, "res", 0.))
                  .getValue<JSNumber>() == 120.);
    }
    */
    SUBCASE("test interpreting function calls with nested callstack") {
        auto source =
            "function multiply(a, b) { var res = a * b; return res;}\nfunction "
            "square(n) { var res = multiply(n,n); return res;}\nfunction "
            "pow2(m) { "
            "var res = square(m);return res;}\n var result = pow2(2);";
        auto lexer       = JSLexer(source);
        auto tokens      = lexer.scanTokens();
        auto parser      = JSParser(std::move(tokens));
        auto stmts       = parser.parse();
        auto interpreter = Interpreter();
        REQUIRE_NOTHROW(interpreter.run(stmts));
        CHECK(
            interpreter.getValue(JSToken(JSTokenKind::Identifier, "result", 0.))
                .getValue<JSNumber>() == 4.);
    }
}

TEST_CASE("testing bytecode compiler") {
    SUBCASE("testing compilation of constant literals") {
        auto source   = "3;";
        auto lexer    = JSLexer(source);
        auto tokens   = lexer.scanTokens();
        auto parser   = JSParser(std::move(tokens));
        auto expr     = parser.parseExpr();
        auto compiler = BytecodeCompiler();
        expr->accept(&compiler);
    }
}

TEST_CASE("testing AST optimizer") {
    SUBCASE("testing constant folding optimizer on binary expressions") {
        auto source = "32 + 10;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        fmt::print("expr::Kind : {}\n", astNodeKindToString(expr->getKind()));
        auto optimizer = ASTOptimizer();
        REQUIRE(expr != nullptr);
        expr->accept(&optimizer);
        expr = optimizer.rewriteAST(expr);
        REQUIRE(expr != nullptr);
        fmt::print("Node kind: {}\n", astNodeKindToString(expr->getKind()));
    }
    SUBCASE("testing constant folding optimizer on non-optimized expression") {
        auto source = "a + 10;";
        auto lexer  = JSLexer(source);
        auto tokens = lexer.scanTokens();
        auto parser = JSParser(std::move(tokens));
        auto expr   = parser.parseExpr();
        fmt::print("expr::Kind : {}\n", astNodeKindToString(expr->getKind()));
        auto optimizer = ASTOptimizer();
        REQUIRE(expr != nullptr);
        expr->accept(&optimizer);
        expr = optimizer.rewriteAST(expr);
        REQUIRE(expr != nullptr);
        fmt::print("Node kind: {}\n", astNodeKindToString(expr->getKind()));
    }
}

TEST_CASE("testing bytecode virtual machine") {
    SUBCASE("testing negate (unary) operation") {
        std::vector<OPCode> bc = {
            OPCode::Constant,
            OPCode(0),
            OPCode::Negate,
            OPCode::Return,
        };
        auto vm = VM(bc);
        vm.storeConstant(JSBasicValue(3.14));

        vm.run();
    }
    SUBCASE("testing add (binary) operation") {
        std::vector<OPCode> bc = {
            OPCode::Constant, OPCode(0),   OPCode::Constant,
            OPCode(1),        OPCode::Add, OPCode::Return,
        };

        auto vm = VM(bc);
        vm.storeConstant(JSBasicValue(3.14));
        vm.storeConstant(JSBasicValue(6.86));

        vm.run();
        CHECK(vm.pop().getValue<JSNumber>() == 10.0);
    }
}
