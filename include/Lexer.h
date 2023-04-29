#ifndef LEXER_H
#define LEXER_H

#include "Token.h"

#include <string>
#include <utility>
#include <vector>

namespace minijsc {

// Check if character is alpha.
auto isAlpha(char chr) -> bool;
// Check if character is digit.
auto isDigit(char chr) -> bool;

// Lexer class implements a lexical analysis module, it consumes text (source)
// and transforms it to a list of tokens.
class Lexer {
    public:
    // Default constructor
    Lexer(std::string source) : source(std::move(source)) {}

    // lex is the main lexer method, calling `lex` will walk the source
    // string and build an `std::vector` of tokens.
    auto lex() -> void;

    // Private functions that are not part of the Lexer's API.

    // Scan multiple tokens.
    auto scanTokens() -> std::vector<Token>;
    // Scan a single token after recognizing its lexeme, add it to the tokens
    // list.
    auto scanToken() -> void;
    // Advance the lexer's cursor position.
    auto advance() -> char;
    // Add a token to the list (for single lexeme tokens).
    auto addToken(TokenType typ) -> void;
    // Check if we reached the end of file.
    auto isAtEnd() -> bool;
    // Match checks if the next token matches the argument.
    auto match(char expected) -> bool;
    // Scan identifiers which are alphanumeric character sequence (string).
    auto scanIdentifier() -> void;
    // Skip whitespace, skips all kinds of whitespace.
    auto skipWhitespace() -> void;
    // Peek returns the current character.
    auto peek() -> char;

    private:
    // Class members declarations
    // Index of starting current token.
    size_t start = 0;
    // Index of the cursor in the source.
    size_t current = 0;
    // Source code we want to lex as an std::string.
    std::string source;
    // List of processed tokens.
    std::vector<Token> tokens;
    // Line in the file or source code we're processing.
    int line = 0;
};

} // namespace minijsc

#endif
