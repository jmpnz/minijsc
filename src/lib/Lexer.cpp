#include "Lexer.h"
#include "Token.h"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace minijsc {

// Advance to the next character.
auto Lexer::advance() -> char {
    auto curr = current;
    current++;
    return source.at(curr);
}

// Append the token to the vector of tokens.
auto Lexer::addToken(TokenType typ) -> void {
    tokens.emplace_back(typ, "", "");
}

// Append the token with its literal value.
auto Lexer::addToken(TokenType typ, std::string literal) {
    auto len  = current - start;
    auto text = source.substr(start, len);
    tokens.emplace_back(typ, text, literal);
}

// Append the token with its numeric value.
auto Lexer::addToken(TokenType typ, double value) {
    auto len  = current - start;
    auto text = source.substr(start, len);
    tokens.emplace_back(typ, text, value);
}

auto Lexer::scanTokens() -> std::vector<Token> {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(TokenType::Eof, "", "");
    return tokens;
}

// Scan the current token.
auto Lexer::scanToken() -> void {
    // skipWhitespace();
    auto nextChar = advance();
    switch (nextChar) {
    case '(':
        addToken(TokenType::LParen);
        break;
    case ')':
        addToken(TokenType::RParen);
        break;
    case '{':
        addToken(TokenType::LBrace);
        break;
    case '}':
        addToken(TokenType::RBrace);
        break;
    case '[':
        addToken(TokenType::LBracket);
        break;
    case ']':
        addToken(TokenType::RBracket);
        break;
    case ',':
        addToken(TokenType::Comma);
        break;
    case '.':
        addToken(TokenType::Dot);
        break;
    case ';':
        addToken(TokenType::Semicolon);
        break;
    case '-':
        addToken(TokenType::Minus);
        break;
    case '+':
        addToken(TokenType::Plus);
        break;
    case '*':
        addToken(TokenType::Star);
        break;
    case '/':
        addToken(TokenType::Slash);
        break;
    case '!': {
        if (match('=')) {
            addToken(TokenType::BangEqual);
            break;
        }
        addToken(TokenType::Bang);
        break;
    }
    case '=': {
        if (match('=')) {
            addToken(TokenType::EqualEqual);
            break;
        }
        addToken(TokenType::Equal);
        break;
    }
    case '<': {
        if (match('=')) {
            addToken(TokenType::LessEqual);
            break;
        }
        addToken(TokenType::Less);
        break;
    }
    case '>': {
        if (match('=')) {
            addToken(TokenType::GreaterEqual);
            break;
        }
        addToken(TokenType::Greater);
        break;
    }
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;
    case '"':
        scanString();
        break;
    default: {
        if (isDigit(nextChar)) {
            scanNumeric();
        } else if (isAlpha(nextChar)) {
            scanIdentifier();
        } else {
            printf("Unexpected token");
        }
        break;
    }
    }
}

// Scan an identifier.
auto Lexer::scanIdentifier() -> void {
    while (isAlphaNumeric(peek())) {
        advance();
    }
    auto len  = current - start;
    auto text = source.substr(start, len);
    auto iter = javascriptKeywords.find(text);
    if (iter == javascriptKeywords.end()) {
        addToken(TokenType::Identifier, text);
        return;
    }
    TokenType typ = iter->second;
    addToken(typ, text);
}

// Scan a numeric.
auto Lexer::scanNumeric() -> void {
    while (isDigit(peek())) {
        advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
    }

    while (isDigit(peek())) {
        advance();
    }

    auto len   = current - start;
    auto value = std::atof(source.substr(start, len).c_str());
    addToken(TokenType::Numeric, value);
}

// Scan a literal string.
auto Lexer::scanString() -> void {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }
    if (isAtEnd()) {
        std::cout << "Unterminated string\n";
        return;
    }
    advance();

    // Build literal.
    auto len   = (current - 1) - (start + 1);
    auto value = source.substr(start + 1, len);
    addToken(TokenType::String, value);
}

// Match if the next token is the one we expect.
auto Lexer::match(char expected) -> bool {
    if (isAtEnd()) {
        return false;
    }
    if (source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
}

// Peek the current character.
auto Lexer::peek() -> char {
    if (isAtEnd()) {
        return '\0';
    }
    return source.at(current);
}

// Peek the next character.
auto Lexer::peekNext() -> char {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source.at(current + 1);
}

// Check if we reached EOF.
auto Lexer::isAtEnd() -> bool { return current >= source.length(); }

// Process source code and build a list of processed tokens.
auto Lexer::lex() -> void { scanToken(); }

// Check if character is alphabet or underscore.
auto isAlpha(char chr) -> bool {
    return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z') ||
           (chr == '_');
}

// Check if character is digit.
auto isDigit(char chr) -> bool { return (chr >= '0' && chr <= '9'); }

// Check if character is alphanumeric.
auto isAlphaNumeric(char chr) -> bool { return (isAlpha(chr) || isDigit(chr)); }

} // namespace minijsc
