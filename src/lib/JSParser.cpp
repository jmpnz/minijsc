#include "JSParser.h"
#include "AST.h"
#include "JSToken.h"
#include "JSValue.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace minijsc {

auto JSParser::parseExpression() -> std::unique_ptr<JSExpr> {
    return parseEqualityExpr();
}

auto JSParser::parsePrimaryExpr() -> std::unique_ptr<JSExpr> {
    if (match(JSTokenKind::False)) {
        return std::make_unique<JSLiteralExpr>(JSBasicValue(false));
    }

    return std::make_unique<JSLiteralExpr>(JSBasicValue(nullptr));
}

auto JSParser::parseEqualityExpr() -> std::unique_ptr<JSExpr> {
    auto expr = parseCmpExpr();

    while (match({JSTokenKind::BangEqual, JSTokenKind::EqualEqual})) {
        JSToken cmpOp = previous();
        auto right    = parseCmpExpr();
        expr = std::make_unique<JSBinExpr>(expr.get(), cmpOp, right.get());
    }

    return expr;
}

} // namespace minijsc
