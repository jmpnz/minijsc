#include "JSParser.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"
#include "fmt/core.h"

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace minijsc {

auto JSParser::parseExpr() -> std::shared_ptr<Expr> {
    fmt::print("JSParser::parseExpr\n");
    return parseEqualityExpr();
}

auto JSParser::parsePrimaryExpr() -> std::shared_ptr<Expr> {
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
    if (match({JSTokenKind::Numeric, JSTokenKind::String})) {
        fmt::print("JSParser::match(Numeric)\n");

        auto literal = previous().getLiteral().getValue<double>();
        fmt::print("Numeric(Literal({}))\n", literal);
        return std::make_shared<JSLiteralExpr>(JSBasicValue(literal));
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

auto JSParser::parseComparisonExpr() -> std::shared_ptr<Expr> {
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

auto JSParser::parseTermExpr() -> std::shared_ptr<Expr> {
    fmt::print("JSParser::parseTermExpr\n");
    auto expr = parseFactorExpr();

    while (match({JSTokenKind::Minus, JSTokenKind::Plus})) {
        auto binOp = previous();
        auto right = parseFactorExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }

    return expr;
}

auto JSParser::parseFactorExpr() -> std::shared_ptr<Expr> {
    fmt::print("JSParser::parseFactorExpr\n");
    auto expr = parseUnaryExpr();

    while (match({JSTokenKind::Slash, JSTokenKind::Star})) {
        auto binOp = previous();
        auto right = parseUnaryExpr();
        expr       = std::make_shared<JSBinExpr>(expr, binOp, right);
    }

    return expr;
}

auto JSParser::parseUnaryExpr() // NOLINT
    -> std::shared_ptr<Expr> {  // NOLINT
    fmt::print("JSParser::parseUnaryExpr\n");
    if (match({JSTokenKind::Bang, JSTokenKind::Minus})) {
        auto unaryOp = previous();
        auto right   = parseUnaryExpr();
        return std::make_shared<JSUnaryExpr>(unaryOp, right);
    }

    return parsePrimaryExpr();
}

auto JSParser::parseEqualityExpr() -> std::shared_ptr<Expr> {
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
