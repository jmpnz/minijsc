#include "JSParser.h"
#include "AST.h"
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

/// Maximum number of arguments supported for functions.
static constexpr int kMaxArgs = 255;

auto JSParser::parse() -> std::vector<std::shared_ptr<JSStmt>> {
    std::vector<std::shared_ptr<JSStmt>> statements;
    while (!isAtEnd()) {
        statements.emplace_back(parseDecl());
    }
    return statements;
}

auto JSParser::parseStmt() -> std::shared_ptr<JSStmt> {
    if (match(JSTokenKind::Function)) {
        return parseFuncDecl();
    }
    if (match(JSTokenKind::If)) {
        return parseIfStmt();
    }
    if (match(JSTokenKind::While)) {
        return parseWhileStmt();
    }
    if (match(JSTokenKind::For)) {
        return parseForStmt();
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

auto JSParser::parseForStmt() -> std::shared_ptr<JSForStmt> {
    // Consume opening parenthesis for the condition block.
    consume(JSTokenKind::LParen, "Expected '(' after for.");
    // Parse the initializer.
    std::shared_ptr<JSStmt> initializer;
    if (match(JSTokenKind::Semicolon)) {
        initializer = nullptr;
    } else if (match(JSTokenKind::Var)) {
        initializer = parseVarDecl();
    } else {
        initializer = parseExprStmt();
    }
    // Parse the condition.
    std::shared_ptr<JSExpr> condition;
    if (!check(JSTokenKind::Semicolon)) {
        condition = parseExpr();
    }
    consume(JSTokenKind::Semicolon, "Expected ';' after condition.");
    // Parse the step.
    std::shared_ptr<JSExpr> step;
    if (!check(JSTokenKind::RParen)) {
        step = parseExpr();
    }
    consume(JSTokenKind::RParen, "Expected ')' after for clause.");
    // Parse the loop body.
    auto body = parseStmt();

    return std::make_shared<JSForStmt>(initializer, condition, step, body);
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

auto JSParser::parseFuncDecl() -> std::shared_ptr<JSStmt> {
    // Consume the token name.
    auto name = consume(JSTokenKind::Identifier,
                        "Expected identifier after function declaration.");
    // Skip the opening parenthesis.
    consume(JSTokenKind::LParen, "Expected '(' after function name.");
    std::vector<JSToken> params;
    // Process the function arguments.
    if (!check(JSTokenKind::RParen)) {
        do {
            if (params.size() >= kMaxArgs) {
                throw std::runtime_error("Can't have more than 255 arguments");
            }
            params.emplace_back(
                consume(JSTokenKind::Identifier, "Expected parameter name"));
        } while (match(JSTokenKind::Comma));
    }
    // Skip the closed parenthesis.
    consume(JSTokenKind::RParen, "Expected ')' after argument list.");
    // Parse the function's body.
    consume(JSTokenKind::LBrace, "Expected '{' after function declaration.");
    auto body = parseBlockStmt();
    return std::make_shared<JSFuncDecl>(name, params, body);
}

auto JSParser::parseExprStmt() -> std::shared_ptr<JSStmt> {
    auto expr = parseExpr();
    if (expr == nullptr) {
        fmt::print("Null expression wut");
        return std::make_shared<JSExprStmt>(expr);
    }
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

    return parseCallExpr();
}

auto JSParser::parseCallExpr() -> std::shared_ptr<JSExpr> {
    fmt::print("JSParser::parseCallExpr\n");
    auto expr = parsePrimaryExpr();

    while (true) {
        if (match(JSTokenKind::LParen)) {
            std::vector<std::shared_ptr<JSExpr>> args;
            if (!check(JSTokenKind::RParen)) {
                do {
                    if (args.size() >= kMaxArgs) {
                        throw std::runtime_error(
                            "Can't have more than 255 arguments.\n");
                    }
                    args.emplace_back(parseExpr());
                } while (match(JSTokenKind::Comma));
            }
            auto paren =
                consume(JSTokenKind::RParen, "Expected ')' after arguments.\n");

            expr = std::make_shared<JSCallExpr>(expr, paren, args);
        } else {
            break;
        }
    }
    return expr;
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
