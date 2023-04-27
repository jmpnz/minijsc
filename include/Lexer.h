#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>

namespace minijsc {

enum class TokenType {
    // Single character tokens
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,

    // Variable declarations
    Var,
    Let,
    Const,

    // Functions and classes
    Function,

    // Control flow
    Return,
    Break,
    Continue,
    Throw,
    If,
    Else,
    Switch,
    Try,
    Catch,

    // Iterations
    Do,
    While,
    For,
    In,
    Of,

    // Literal booleans
    True,
    False,
};

// Token class represents the outputs of our lexer, a token has an associated
// type, encoded as an enum, a lexeme that holds the textual representation
// of the token and a literal (for numerics, strings and identifiers).
// TODO: move Token to its own file.
class Token {
    public:
    // Default constructor
    Token(TokenType t, std::string lexeme, std::string literal):
        type(t), lexeme(lexeme), literal(literal) {}

    // Return a textual representation of the token.
    [[nodiscard]] auto toString() const -> std::string;
    private:
    // Token type
    TokenType type;
    // Lexeme is the string representation of the token.
    std::string lexeme;
    // Literal is the literal value associated with some token types
    // such as numeric values, strings or identifiers.
    std::string literal;
};

// Lexer class implements a lexical analysis module, it consumes text (source)
// and transforms it to a list of tokens.
class Lexer {
    public:
    // Default constructor
    Lexer(std::string source):source(source) {}
    // lex is the main lexer method, calling `lex` will walk the source
    // string and build an `std::vector` of tokens.
    auto lex() -> void;
    private:
    // Private functions that are not part of the Lexer's API.

    // Scan a single token after recognizing its lexeme, add it to the tokens
    // list.
    auto scanToken() -> void;
    // Advance the lexer's cursor position.
    auto advance() -> char;
    // Add a token to the list (for single lexeme tokens).
    auto addToken(TokenType t) -> void;

    // Class members declarations
    // Index of the cursor in the source.
    size_t current = 0;
    // Source code we want to lex as an std::string.
    std::string source;
    // List of processed tokens.
    std::vector<Token> tokens;
};

}// namespace minijsc

#endif
