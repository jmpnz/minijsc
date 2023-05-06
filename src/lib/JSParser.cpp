#include "JSParser.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace minijsc {

auto JSParser::parsePrimaryExpr() -> std::unique_ptr<Expr<JSBasicValue>> {
    if (match(JSTokenKind::False)) {
        return std::make_unique<JSLiteralExpr>(JSBasicValue(false));
    }
    if (match(JSTokenKind::True)) {
        return std::make_unique<JSLiteralExpr>(JSBasicValue(true));
    }
    if (match(JSTokenKind::Null)) {
        return std::make_unique<JSLiteralExpr>(JSBasicValue(nullptr));
    }
    if (match({JSTokenKind::Numeric, JSTokenKind::String})) {
        return std::make_unique<JSLiteralExpr>(
            JSBasicValue(previous().getLiteral()));
    }

    return std::make_unique<JSLiteralExpr>(JSBasicValue(nullptr));
}

auto JSParser::parseComparisonExpr() -> std::unique_ptr<Expr<JSBasicValue>> {
    auto expr = parseTermExpr();

    while (match({JSTokenKind::Greater, JSTokenKind::GreaterEqual,
                  JSTokenKind::Less, JSTokenKind::LessEqual})) {
        auto binOp = previous();
        auto right = parseTermExpr();
        expr = std::make_unique<JSBinExpr>(expr.get(), binOp, right.get());
    }
    return expr;
}

auto JSParser::parseTermExpr() -> std::unique_ptr<Expr<JSBasicValue>> {
    auto expr = parseFactorExpr();

    while (match({JSTokenKind::Minus, JSTokenKind::Plus})) {
        auto binOp = previous();
        auto right = parseFactorExpr();
        expr = std::make_unique<JSBinExpr>(expr.get(), binOp, right.get());
    }

    return expr;
}

auto JSParser::parseFactorExpr() -> std::unique_ptr<Expr<JSBasicValue>> {
    auto expr = parseUnaryExpr();

    while (match({JSTokenKind::Slash, JSTokenKind::Star})) {
        auto binOp = previous();
        auto right = parseUnaryExpr();
        expr = std::make_unique<JSBinExpr>(expr.get(), binOp, right.get());
    }

    return expr;
}

auto JSParser::parseUnaryExpr()              // NOLINT
    -> std::unique_ptr<Expr<JSBasicValue>> { // NOLINT
    if (match({JSTokenKind::Bang, JSTokenKind::Minus})) {
        auto unaryOp = previous();
        auto right   = parseUnaryExpr();
        return std::make_unique<JSUnaryExpr>(unaryOp, right.get());
    }

    return parsePrimaryExpr();
}

auto JSParser::parseEqualityExpr() -> std::unique_ptr<Expr<JSBasicValue>> {
    auto expr = parseComparisonExpr();

    while (match({JSTokenKind::BangEqual, JSTokenKind::EqualEqual})) {
        auto binOp = previous();
        auto right = parseComparisonExpr();
        expr = std::make_unique<JSBinExpr>(expr.get(), binOp, right.get());
    }

    return expr;
}
} // namespace minijsc
