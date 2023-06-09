#include "fmt/color.h"
#include "fmt/core.h"

#include "JSLexer.h"
#include "JSToken.h"
#include "JSValue.h"

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
auto JSLexer::advance() -> char {
    auto curr = current;
    current++;
    return source.at(curr);
}

// Append a token without a lexeme to the vector of tokens.
auto JSLexer::addToken(JSTokenKind kind) -> void {
    tokens.emplace_back(kind, "", "");
}

// Append the token with its literal value (String).
auto JSLexer::addToken(JSTokenKind kind, const std::string& literal) {
    auto len  = current - start;
    auto text = source.substr(start, len);
    tokens.emplace_back(kind, text, literal);
}

// Append the token with its literal value (Numeric).
auto JSLexer::addToken(JSTokenKind kind, double literal) {
    auto len  = current - start;
    auto text = source.substr(start, len);
    tokens.emplace_back(kind, text, literal);
}

// Core lexer scanning function.
auto JSLexer::scanTokens() -> std::vector<JSToken> {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.emplace_back(JSTokenKind::Eof, "", "");
    return tokens;
}

// Scan the current character and process its token.
auto JSLexer::scanToken() -> void {
    // Advance the cursor position.
    auto nextChar = advance();
    // Match the current char against supported lexemes.
    switch (nextChar) {
    case '(':
        addToken(JSTokenKind::LParen);
        break;
    case ')':
        addToken(JSTokenKind::RParen);
        break;
    case '{':
        addToken(JSTokenKind::LBrace);
        break;
    case '}':
        addToken(JSTokenKind::RBrace);
        break;
    case '[':
        addToken(JSTokenKind::LBracket);
        break;
    case ']':
        addToken(JSTokenKind::RBracket);
        break;
    case ',':
        addToken(JSTokenKind::Comma);
        break;
    case '.':
        addToken(JSTokenKind::Dot);
        break;
    case ';':
        addToken(JSTokenKind::Semicolon);
        break;
    case '-':
        addToken(JSTokenKind::Minus);
        break;
    case '+':
        addToken(JSTokenKind::Plus);
        break;
    case '*':
        addToken(JSTokenKind::Star);
        break;
    case '/':
        addToken(JSTokenKind::Slash);
        break;
    case '!': {
        if (match('=')) {
            addToken(JSTokenKind::BangEqual);
            break;
        }
        addToken(JSTokenKind::Bang);
        break;
    }
    case '=': {
        if (match('=')) {
            addToken(JSTokenKind::EqualEqual);
            break;
        }
        addToken(JSTokenKind::Equal);
        break;
    }
    case '<': {
        if (match('=')) {
            addToken(JSTokenKind::LessEqual);
            break;
        }
        addToken(JSTokenKind::Less);
        break;
    }
    case '&': {
        if (match('&')) {
            addToken(JSTokenKind::And);
            break;
        }
        break;
    }
    case '|': {
        if (match('|')) {
            addToken(JSTokenKind::Or);
            break;
        }
        break;
    }
    case '>': {
        if (match('=')) {
            addToken(JSTokenKind::GreaterEqual);
            break;
        }
        addToken(JSTokenKind::Greater);
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
            fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                       "Unexpected token: {}\n", nextChar);
        }
        break;
    }
    }
}

// Scan an identifier.
auto JSLexer::scanIdentifier() -> void {
    while (isAlphaNumeric(peek())) {
        advance();
    }
    // Get current lexeme length.
    auto len = current - start;
    // Extract the lexeme.
    auto text = source.substr(start, len);
    // Check if the lexeme is a keyword, if so process it as a keyword.
    if (auto iter = jsKeywords.find(text); iter != jsKeywords.end()) {
        addToken(iter->second, text);
        return;
    }
    // Lexeme isn't a keyword, must be an identifier.
    addToken(JSTokenKind::Identifier, text);
}

// Scan a numeric.
auto JSLexer::scanNumeric() -> void {
    // While processing digits, consume the character lexemes one by one.
    while (isDigit(peek())) {
        advance();
    }
    // If we find a dot lexeme we consume it then move to consume
    // the decimal section.
    if (peek() == '.' && isDigit(peekNext())) {
        advance();
    }
    // Consume decimal section digits.
    while (isDigit(peek())) {
        advance();
    }
    // Get the numeric lexeme length.
    auto len = current - start;
    // Extract the numeric lexeme from source and cast it as a double value.
    auto value = std::atof(source.substr(start, len).c_str());
    // Append the numeric token to the tokens vector.
    addToken(JSTokenKind::Numeric, value);
}

// Scan a literal string.
auto JSLexer::scanString() -> void {
    while (peek() != '"' && !isAtEnd()) {
        // Some strings may span multiple lines.
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
    // For a string literal such as "hello"
    // after consume the characters in the lexeme
    // current would point to the closing quote
    // start would point to the starting quote
    auto len   = (current - 1) - (start + 1);
    auto value = source.substr(start + 1, len);
    addToken(JSTokenKind::String, value);
}

// Match if the current character is the one we expect.
auto JSLexer::match(char expected) -> bool {
    if (isAtEnd()) {
        return false;
    }
    if (source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
}

// Peek the current character, unlike advance, peek is a non-consuming single
// character lookahead.
auto JSLexer::peek() -> char {
    if (isAtEnd()) {
        return '\0';
    }
    return source.at(current);
}

// Peek the next character.
auto JSLexer::peekNext() -> char {
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source.at(current + 1);
}

// Check if we reached EOF.
auto JSLexer::isAtEnd() -> bool { return current >= source.length(); }

// Process source code and build a list of processed tokens.
auto JSLexer::lex() -> void { scanTokens(); }

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
