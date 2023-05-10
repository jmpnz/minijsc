#include "AST.h"
#include "JSParser.h"
#include "JSToken.h"
#include "JSValue.h"
#include "fmt/core.h"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace minijsc {

auto JSParser::parse() -> std::vector<std::shared_ptr<JSStmt>> {
    std::vector<std::shared_ptr<JSStmt>> statements;
    while (!isAtEnd()) {
        statements.emplace_back(parseDecl());
    }
    return statements;
}

auto JSParser::parseStmt() -> std::shared_ptr<JSStmt> {
    if (match(JSTokenKind::If)) {
        return parseIfStmt();
    }
    if (match(JSTokenKind::While)) {
        return parseWhileStmt();
    }
    if (match(JSTokenKind::LBrace)) {
        return parseBlockStmt();
    }
    return parseExprStmt();
}

auto JSParser::parseBlockStmt() -> std::shared_ptr<JSBlockStmt> {
    std::vector<std::shared_ptr<JSStmt>> statements;

    while (!check(JSTokenKind::RBrace) && !isAtEnd()) {
        statements.emplace_back(parseDecl());
    }

    consume(JSTokenKind::RBrace, "Expected '}' after block.");
    return std::make_shared<JSBlockStmt>(statements);
}

auto JSParser::parseIfStmt() -> std::shared_ptr<JSIfStmt> {
    // Consume opening parenthesis for the condition block.
    consume(JSTokenKind::LParen, "Expected '(' after if.");
    // Parse conditional expression.
    auto condition = parseExpr();
    // Consume closing parenthesis for the condition block.
    consume(JSTokenKind::RParen, "Expected ')' after expression.");
    // Parse the then branch statement.
    auto thenBranch = parseStmt();
    if (match(JSTokenKind::Else)) {
        auto elseBranch = parseStmt();
        return std::make_shared<JSIfStmt>(condition, thenBranch, elseBranch);
    }
    // If there's no else branch we set the else branch statement to nullptr.
    return std::make_shared<JSIfStmt>(condition, thenBranch, nullptr);
}

auto JSParser::parseWhileStmt() -> std::shared_ptr<JSWhileStmt> {
    // Consume opening parenthesis for the condition block.
    consume(JSTokenKind::LParen, "Expected '(' after while.");
    // Parse conditional expression.
    auto condition = parseExpr();
    // Consume closing parenthesis for the condition block.
    consume(JSTokenKind::RParen, "Expected ')' after expression.");
    // Parse statement body.
    auto body = parseStmt();
    // Create AST node for while statement.
    return std::make_shared<JSWhileStmt>(condition, body);
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
    fmt::print("parseExprStmt::\n");
    fmt::print("Node Kind : {}\n", astNodeKindToString(expr->getKind()));
    fmt::print("Current Token: {}\n", tokens[current].toString());
    fmt::print("Peek Token: {}\n", tokens[current + 1].toString());
    consume(JSTokenKind::Semicolon, "Expected ';' after expression");
    return std::make_shared<JSExprStmt>(expr);
}

auto JSParser::parseExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseExpr\n");
    return parseAssignmentExpr();
}

auto JSParser::parseAssignmentExpr() -> std::shared_ptr<JSExpr> {
    auto expr = parseEqualityExpr();

    if (match(JSTokenKind::Equal)) {
        auto equals = previous();
        auto value  = parseAssignmentExpr();

        // We could use some form of RTTI to decide
        // the right handside which could be a variable
        // expression or a function expression.
        // But since our AST node encode their node kind
        // we can simply check the node kind.
        if (expr->getKind() == ASTNodeKind::VarExpr) {
            fmt::print("JSParaser::parseAssignment::IsVarExpr\n");
            auto name = std::static_pointer_cast<JSVarExpr>(expr)->getName();
            return std::make_shared<JSAssignExpr>(name, value);
        }

        throw std::runtime_error("Invalid assignment target.");
    }

    return expr;
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
        auto literal = previous().getLiteral();
        return std::make_shared<JSLiteralExpr>(literal);
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
