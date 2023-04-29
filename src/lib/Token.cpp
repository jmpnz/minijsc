#include "Token.h"

namespace minijsc {

auto Token::toString() const -> std::string {
    // TODO: implement remaining tokens
    switch (type) {
    case TokenType::LParen:
        return "(";
    case TokenType::RParen:
        return ")";
    case TokenType::LBrace:
        return "{";
    case TokenType::RBrace:
        return "}";
    case TokenType::LBracket:
        return "[";
    case TokenType::RBracket:
        return "]";
    case TokenType::Comma:
        return ",";
    case TokenType::Dot:
        return ".";
    case TokenType::Minus:
        return "-";
    case TokenType::Plus:
        return "+";
    case TokenType::Semicolon:
        return ";";
    case TokenType::Slash:
        return "/";
    case TokenType::Star:
        return "*";
    case TokenType::Equal:
        return "=";
    case TokenType::EqualEqual:
        return "==";
    case TokenType::Less:
        return "<";
    case TokenType::LessEqual:
        return "<=";
    case TokenType::GreaterEqual:
        return ">=";
    case TokenType::Greater:
        return ">";
    case TokenType::Let:
        return "LET";
    case TokenType::Var:
        return "VAR";
    case TokenType::Const:
        return "CONST";
    case TokenType::Function:
        return "FUNCTION";
    case TokenType::Return:
        return "RETURN";
    case TokenType::Break:
        return "BREAK";
    case TokenType::Continue:
        return "CONTINUE";
    case TokenType::Throw:
        return "THROW";
    case TokenType::If:
        return "IF";
    case TokenType::Else:
        return "ELSE";
    case TokenType::Switch:
        return "SWITCH";
    case TokenType::Try:
        return "TRY";
    case TokenType::Catch:
        return "CATCH";
    case TokenType::Do:
        return "DO";
    case TokenType::While:
        return "WHILE";
    case TokenType::For:
        return "FOR";
    case TokenType::In:
        return "IN";
    case TokenType::Of:
        return "Of";
    case TokenType::True:
        return "TRUE";
    case TokenType::False:
        return "FALSE";
    default:
        return "UNKNOWN TOKEN";
    }
}

}; // namespace minijsc
