#ifndef TOKEN_H
#define TOKEN_H

#include "JSValue.h"

#include <string>
#include <utility>
#include <variant>

namespace minijsc {
// JavaScript token types.
enum class JSTokenKind {
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

    // Notable keywords
    Import,
    Export,
    Default,

    // Variable declarations
    Var,
    Let,
    Const,

    // Functions and classes
    Function,
    Class,
    Super,
    Extends,
    Delete,
    InstanceOf,
    TypeOf,
    New,
    This,

    // Control flow
    Return,
    Break,
    Continue,
    Throw,
    If,
    Else,
    Switch,
    Case,
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
    Null,
    Void,

    // End of file.
    Eof,
};

// Token class represents the outputs of our lexer, a token has an associated
// kind, encoded as an enum, a lexeme that holds the textual representation
// of the token and a literal (for numerics, strings and identifiers).
class JSToken {
    public:
    // Default constructor.
    explicit JSToken(JSTokenKind typ, std::string lexeme,
                     JSBasicValue literal)                          // NOLINT
        : kind(typ), lexeme(std::move(lexeme)), literal(literal) {} // NOLINT

    // Default constructo for numeric values.
    JSToken(JSTokenKind typ, std::string lexeme,
            double literal)                                         // NOLINT
        : kind(typ), lexeme(std::move(lexeme)), literal(literal) {} // NOLINT

    // Return a textual representation of the token.
    [[nodiscard]] auto toString() const -> std::string;

    // Return the token kind.
    [[nodiscard]] auto getKind() const -> JSTokenKind { return kind; }

    // Return the value literal.
    [[nodiscard]] auto getLiteral() const -> JSBasicValue { return literal; }

    // Return the lexeme.
    [[nodiscard]] auto getLexeme() const -> std::string { return lexeme; }

    private:
    // Token kind
    JSTokenKind kind;
    // Lexeme is the string representation of the token.
    std::string lexeme;
    // Literal is the literal value associated with some token types
    // such as numeric values, strings or identifiers.
    JSBasicValue literal;
};

}; // namespace minijsc

#endif
