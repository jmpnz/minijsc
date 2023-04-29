#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <variant>

namespace minijsc {
// JavaScript token types.
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
    // Can be multicharacter token.
    Bang,
    Equal,
    Less,
    Greater,

    // Double character tokens
    BangEqual,
    EqualEqual,
    LessEqual,
    GreaterEqual,

    // Multicharacter tokens (identifiers, strings, literals).
    Identifier,
    Numeric,
    String,

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

    // End of file.
    Eof,
};

// Token class represents the outputs of our lexer, a token has an associated
// type, encoded as an enum, a lexeme that holds the textual representation
// of the token and a literal (for numerics, strings and identifiers).
class Token {
    public:
    // Default constructor
    Token(TokenType typ, std::string lexeme, std::string literal) //NOLINT
        : type(typ), lexeme(lexeme), literal(literal) {}          //NOLINT

    // Return a textual representation of the token.
    [[nodiscard]] auto toString() const -> std::string;

    // Return token type.
    auto GetType() -> TokenType { return type; }

    private:
    // Token type
    TokenType type;
    // Lexeme is the string representation of the token.
    std::string lexeme;
    // Literal is the literal value associated with some token types
    // such as numeric values, strings or identifiers.
    std::variant<double, std::string> literal;
};

}; // namespace minijsc

#endif
