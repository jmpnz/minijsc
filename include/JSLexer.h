#ifndef LEXER_H
#define LEXER_H

#include "JSToken.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace minijsc {

// Check if character is alpha.
auto isAlpha(char chr) -> bool;
// Check if character is digit.
auto isDigit(char chr) -> bool;
// Check if charcter is alphanumeric.
auto isAlphaNumeric(char chr) -> bool;

// JavaScript keywords.
static std::unordered_map<std::string, JSTokenKind> jsKeywords = {
    {"let", JSTokenKind::Let},
    {"var", JSTokenKind::Var},
    {"const", JSTokenKind::Const},
    {"function", JSTokenKind::Function},
    {"class", JSTokenKind::Class},
    {"super", JSTokenKind::Super},
    {"this", JSTokenKind::This},
    {"extends", JSTokenKind::Extends},
    {"delete", JSTokenKind::Delete},
    {"instanceof", JSTokenKind::InstanceOf},
    {"typeof", JSTokenKind::TypeOf},
    {"new", JSTokenKind::New},
    {"import", JSTokenKind::Import},
    {"export", JSTokenKind::Export},
    {"default", JSTokenKind::Default},
    {"return", JSTokenKind::Return},
    {"continue", JSTokenKind::Continue},
    {"break", JSTokenKind::Break},
    {"if", JSTokenKind::If},
    {"else", JSTokenKind::Else},
    {"switch", JSTokenKind::Switch},
    {"case", JSTokenKind::Case},
    {"throw", JSTokenKind::Throw},
    {"try", JSTokenKind::Try},
    {"catch", JSTokenKind::Catch},
    {"do", JSTokenKind::Do},
    {"while", JSTokenKind::While},
    {"for", JSTokenKind::For},
    {"in", JSTokenKind::In},
    {"of", JSTokenKind::Of},
    {"true", JSTokenKind::True},
    {"false", JSTokenKind::False},
    {"null", JSTokenKind::Null},
};

// Lexer class implements a lexical analysis module, it consumes text (source)
// and transforms it to a list of tokens.
class JSLexer {
    public:
    // Default constructor
    explicit JSLexer(std::string source) : source(std::move(source)) {}

    // Run the lexical analysis pass, populating the vector of tokens.
    auto lex() -> void;

    // Return the internal vector of tokens.
    auto getTokens() -> std::vector<JSToken> { return tokens; }

    // Scan the source code and populate the the vector of tokens, the function
    // acts as a proxy for `lex`.
    auto scanTokens() -> std::vector<JSToken>;
    // Single token lexical pass
    auto scanToken() -> void;
    // Advance the lexer's cursor position.
    auto advance() -> char;
    // Add a token to the list (for single lexeme tokens).
    auto addToken(JSTokenKind typ) -> void;
    // Add a token with a literal value.
    auto addToken(JSTokenKind typ, const std::string& literal);
    // Add a token with a numeric value.
    auto addToken(JSTokenKind typ, double literal);
    // Check if we reached the end of file.
    auto isAtEnd() -> bool;
    // Match checks if the next token matches the argument.
    auto match(char expected) -> bool;
    // Scan identifiers which are alphanumeric character sequence (string).
    auto scanIdentifier() -> void;
    // Scan numeric literals.
    auto scanNumeric() -> void;
    // Scan literal strings.
    auto scanString() -> void;
    // Skip whitespace, skips all kinds of whitespace.
    auto skipWhitespace() -> void;
    // Peek returns the current character.
    auto peek() -> char;
    // Peek returns the next character.
    auto peekNext() -> char;

    private:
    // Index of starting current token.
    size_t start = 0;
    // Index of the cursor in the source.
    size_t current = 0;
    // Source code we want to lex as an std::string.
    std::string source;
    // List of processed tokens.
    std::vector<JSToken> tokens;
    // Line in the file or source code we're processing.
    int line = 0;
};

} // namespace minijsc

#endif
