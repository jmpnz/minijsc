#include "Token.h"

namespace minijsc {

// Return a string representation of the token based on its type.
auto Token::toString() const -> std::string {
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
    case TokenType::Bang:
        return "!";
    case TokenType::BangEqual:
        return "!=";
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
    case TokenType::Class:
        return "CLASS";
    case TokenType::This:
        return "THIS";
    case TokenType::Extends:
        return "EXTENDS";
    case TokenType::Super:
        return "SUPER";
    case TokenType::Default:
        return "DEFAULT";
    case TokenType::TypeOf:
        return "TYPEOF";
    case TokenType::InstanceOf:
        return "INSTANCEOF";
    case TokenType::Delete:
        return "DELETE";
    case TokenType::New:
        return "NEW";
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
    case TokenType::Case:
        return "CASE";
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
    case TokenType::Null:
        return "NULL";
    case TokenType::Import:
        return "IMPORT";
    case TokenType::Export:
        return "EXPORT";
    case TokenType::String:
        return "STRING(" + std::get<std::string>(literal) + ")";
    case TokenType::Identifier:
        return "IDENTIFIER(" + std::get<std::string>(literal) + ")";
    case TokenType::Numeric:
        return "NUMERIC(" + std::to_string(std::get<double>(literal)) + ")";
    case TokenType::Eof:
        return "EOF";
    }

    return "UNKNOWN TOKEN";
}

}; // namespace minijsc
