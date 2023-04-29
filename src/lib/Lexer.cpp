#include "Lexer.h"
#include "Token.h"

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

namespace minijsc {

// Advance to the next character.
auto Lexer::advance() -> char { return source.at(current++); }

// Append the token to the vector of tokens.
auto Lexer::addToken(TokenType typ) -> void {
    auto lexeme = source.substr(start, current);
    auto tok    = Token(typ, lexeme, "");
    tokens.emplace_back(typ, "", "");
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
    default:
        if (isAlpha(nextChar)) {
            // Read an alphanumeric string, includes underscores.
            scanIdentifier();
            break;
        } else if (isDigit(nextChar)) {
        }

        printf("Unexpected token");
    }
}

auto Lexer::scanIdentifier() -> void {
    while (isAlpha(peek())) {
        advance();
    }
    addToken(TokenType::Identifier);
}

// Skip whitespace.
//auto Lexer::skipWhitespace() -> void {
//    for (;;) {
//        char nextChar = peek();
//        switch (nextChar) {
//        case ' ':
//        case '\r':
//        case '\t':
//            advance();
//            break;
//        case '\n':
//            line++;
//            advance();
//            break;
//        }
//    }
//}

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

} // namespace minijsc
