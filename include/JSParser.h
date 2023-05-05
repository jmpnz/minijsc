#ifndef JSPARSER_H
#define JSPARSER_H

#include "JSToken.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace minijsc {

/// JSParser is a recursive descent parser that builds the abstract syntax
/// tree for the JavaScript source.
class JSParser {
    /// Constructor takes the output of the lexer.
    explicit JSParser(std::vector<JSToken> tokens)
        : tokens(std::move(tokens)) {}

    // Match the next token against what we expect.
    auto match(const JSToken& expected) -> bool;

    // Advance consumes the current token and returns it.
    auto advance() -> JSToken {
        if (!isAtEnd()) {
            current++;
        }
        return previous();
    }

    // Check if we reached the end.
    auto isAtEnd() -> bool { return peek().getKind() == JSTokenKind::Eof; }

    // Peek the current token we're at currently.
    auto peek() -> JSToken { return tokens.at(current); }

    // Return the most recently consumed token.
    auto previous() -> JSToken { return tokens.at(current - 1); }

    private:
    // Tokens the parser is processing.
    std::vector<JSToken> tokens;
    // Current token the parser is point to.
    std::size_t current = 0;
};

} // namespace minijsc

#endif
