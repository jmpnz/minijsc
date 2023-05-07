#include "JSParser.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"
#include "fmt/core.h"

#include <cassert>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace minijsc {

auto JSParser::parseStmt() -> std::shared_ptr<JSStmt> {
    return parseExprStmt();
}

auto JSParser::parseDecl() -> std::shared_ptr<JSStmt> {
    if (match(JSTokenKind::Var)) {
        fmt::print("JSParser::parseVarDecl\n");
        return parseVarDecl();
    }
    return parseStmt();
}

auto JSParser::parseVarDecl() -> std::shared_ptr<JSStmt> {
    auto name =
        consume(JSTokenKind::Identifier, "Expected identifier after var");
    if (match(JSTokenKind::Equal)) {
        auto initializer = parseExpr();
        consume(JSTokenKind::Semicolon,
                "Expected ; after variable declaration");
        return std::make_shared<JSVarDecl>(name, initializer);
    }
    consume(JSTokenKind::Semicolon, "Expected ; after variable declaration");
    return std::make_shared<JSVarDecl>(name);
}

auto JSParser::parseExprStmt() -> std::shared_ptr<JSStmt> {
    auto expr = parseExpr();
    consume(JSTokenKind::Semicolon, "Expected ';' after expression");
    return std::make_shared<JSExprStmt>(expr);
}

auto JSParser::parseExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseExpr\n");
    return parseEqualityExpr();
}

auto JSParser::parsePrimaryExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parsePrimaryExpr\n");
    if (match(JSTokenKind::False)) {
        return std::make_shared<JSLiteralExpr>(JSBasicValue(false));
    }
    if (match(JSTokenKind::True)) {
        return std::make_shared<JSLiteralExpr>(JSBasicValue(true));
    }
    if (match(JSTokenKind::Null)) {
        return std::make_shared<JSLiteralExpr>(JSBasicValue(nullptr));
    }
    if (match({JSTokenKind::Numeric, JSTokenKind::String,
               JSTokenKind::Undefined, JSTokenKind::Null})) {
        fmt::print("JSParser::match(Numeric)\n");

        auto literal = previous().getLiteral().getValue<double>();
        fmt::print("Numeric(Literal({}))\n", literal);
        return std::make_shared<JSLiteralExpr>(JSBasicValue(literal));
    }
    if (match(JSTokenKind::Identifier)) {
        fmt::print("JSParse::match(Identifier)");
        return std::make_shared<JSVarExpr>(previous());
    }

    if (match(JSTokenKind::LParen)) {
        fmt::print("JSParser::match(LParen)\n");
        auto expr = parseExpr();
        consume(JSTokenKind::RParen, "Expected ')' after expression");
        fmt::print("JSParser::match(RParen)\n");
        return std::make_shared<JSGroupingExpr>(expr);
    }

    return nullptr;
}

auto JSParser::parseComparisonExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseComparisonExpr\n");
    auto expr = parseTermExpr();

    while (match({JSTokenKind::Greater, JSTokenKind::GreaterEqual,
                  JSTokenKind::Less, JSTokenKind::LessEqual})) {
        auto binOp = previous();
        auto right = parseTermExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }
    return expr;
}

auto JSParser::parseTermExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseTermExpr\n");
    auto expr = parseFactorExpr();

    while (match({JSTokenKind::Minus, JSTokenKind::Plus})) {
        auto binOp = previous();
        auto right = parseFactorExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }

    return expr;
}

auto JSParser::parseFactorExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseFactorExpr\n");
    auto expr = parseUnaryExpr();

    while (match({JSTokenKind::Slash, JSTokenKind::Star})) {
        auto binOp = previous();
        auto right = parseUnaryExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }

    return expr;
}

auto JSParser::parseUnaryExpr()  // NOLINT
    -> std::shared_ptr<JSExpr> { // NOLINT
    fmt::print("JSParser::parseUnaryExpr\n");
    if (match({JSTokenKind::Bang, JSTokenKind::Minus})) {
        fmt::print("JSParser::match(Bang, Minus)\n");
        auto unaryOp = previous();
        assert(unaryOp.getKind() == JSTokenKind::Bang ||
               unaryOp.getKind() == JSTokenKind::Minus);
        fmt::print("Token : {}\n", unaryOp.getLexeme());
        auto right = parseUnaryExpr();
        fmt::print("Right : {}\n",
                   right->getKind() == ASTNodeKind::LiteralExpr);
        return std::make_shared<JSUnaryExpr>(unaryOp, right);
    }

    return parsePrimaryExpr();
}

auto JSParser::parseEqualityExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseEqualityExpr\n");
    auto expr = parseComparisonExpr();

    while (match({JSTokenKind::BangEqual, JSTokenKind::EqualEqual})) {
        auto binOp = previous();
        auto right = parseComparisonExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }

    return expr;
}
} // namespace minijsc
