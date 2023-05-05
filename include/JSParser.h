//===----------------------------------------------------------------------===//
// JSParser.h: This header defines the class and interface for the minijsc
// parser. The parser we implement is a handwritten recursive descent parser.
//===----------------------------------------------------------------------===//
#ifndef JSPARSER_H
#define JSPARSER_H

#include "AST.h"
#include "JSToken.h"

#include <algorithm>
#include <cstddef>
#include <list>
#include <ranges>
#include <utility>
#include <vector>

namespace minijsc {

/// JSParser is a recursive descent parser that builds the abstract syntax
/// tree for the JavaScript source.
class JSParser {
    public:
    /// Constructor takes the output of the lexer.
    explicit JSParser(std::vector<JSToken> tokens)
        : tokens(std::move(tokens)) {}

    // Match the next token against what we expect.
    auto match(const JSTokenKind& expected) -> bool {
        return tokens.at(current).getKind() == expected;
    }

    // Match one of many of the expected tokens.
    auto match(const std::list<JSTokenKind>& expected) -> bool {
        return std::any_of(expected.begin(), expected.end(),
                           [this](JSTokenKind tok) {
                               if (check(tok)) {
                                   advance();
                                   return true;
                               }
                               return false;
                           });
    }

    // Advance consumes the current token and returns it.
    auto advance() -> JSToken {
        if (!isAtEnd()) {
            current++;
        }
        return previous();
    }

    // Check if the current token matches the passed kind.
    auto check(const JSTokenKind& kind) -> bool {
        if (isAtEnd()) {
            return false;
        }
        return peek().getKind() == kind;
    }

    // Check if we reached the end.
    auto isAtEnd() -> bool { return peek().getKind() == JSTokenKind::Eof; }

    // Peek the current token we're at currently.
    auto peek() -> JSToken { return tokens.at(current); }

    // Return the most recently consumed token.
    auto previous() -> JSToken { return tokens.at(current - 1); }

    // Parse an expression.
    auto parseExpression() -> JSExpr;

    // Parse an equality expression.
    auto parseEqualityExpr() -> JSExpr;

    private:
    // Tokens the parser is processing.
    std::vector<JSToken> tokens;
    // Current token the parser is point to.
    std::size_t current = 0;
};

} // namespace minijsc

#endif
