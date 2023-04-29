#ifndef LEXER_H
#define LEXER_H

#include "Token.h"

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

// Map of keywords.
static std::unordered_map<std::string, TokenType> Keywords = {
    {"let", TokenType::Let},
    {"var", TokenType::Var},
    {"const", TokenType::Const},
    {"function", TokenType::Function},
    {"class", TokenType::Class},
    {"super", TokenType::Super},
    {"this", TokenType::This},
    {"extends", TokenType::Extends},
    {"delete", TokenType::Delete},
    {"instanceOf", TokenType::InstanceOf},
    {"typeOf", TokenType::TypeOf},
    {"new", TokenType::New},
    {"import", TokenType::Import},
    {"export", TokenType::Export},
    {"default", TokenType::Default},
    {"return", TokenType::Return},
    {"continue", TokenType::Continue},
    {"break", TokenType::Break},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"switch", TokenType::Switch},
    {"case", TokenType::Case},
    {"throw", TokenType::Throw},
    {"try", TokenType::Try},
    {"catch", TokenType::Catch},
    {"do", TokenType::Do},
    {"while", TokenType::While},
    {"for", TokenType::For},
    {"in", TokenType::In},
    {"of", TokenType::Of},
    {"true", TokenType::True},
    {"false", TokenType::False},
    {"null", TokenType::Null},
};

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
    // Add a token with a literal value.
    auto addToken(TokenType typ, std::string literal);
    // Add a token with a numeric value.
    auto addToken(TokenType typ, double value);
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
