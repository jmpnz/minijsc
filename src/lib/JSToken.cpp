#include "JSToken.h"
#include <string>

namespace minijsc {

// Return a string representation of the token based on its kind.
auto JSToken::toString() const -> std::string {
    switch (kind) {
    case JSTokenKind::LParen:
        return "(";
    case JSTokenKind::RParen:
        return ")";
    case JSTokenKind::LBrace:
        return "{";
    case JSTokenKind::RBrace:
        return "}";
    case JSTokenKind::LBracket:
        return "[";
    case JSTokenKind::RBracket:
        return "]";
    case JSTokenKind::Bang:
        return "!";
    case JSTokenKind::BangEqual:
        return "!=";
    case JSTokenKind::Comma:
        return ",";
    case JSTokenKind::Dot:
        return ".";
    case JSTokenKind::Minus:
        return "-";
    case JSTokenKind::Plus:
        return "+";
    case JSTokenKind::Semicolon:
        return ";";
    case JSTokenKind::Slash:
        return "/";
    case JSTokenKind::Star:
        return "*";
    case JSTokenKind::Equal:
        return "=";
    case JSTokenKind::EqualEqual:
        return "==";
    case JSTokenKind::Less:
        return "<";
    case JSTokenKind::LessEqual:
        return "<=";
    case JSTokenKind::GreaterEqual:
        return ">=";
    case JSTokenKind::Greater:
        return ">";
    case JSTokenKind::Let:
        return "LET";
    case JSTokenKind::Var:
        return "VAR";
    case JSTokenKind::Const:
        return "CONST";
    case JSTokenKind::Function:
        return "FUNCTION";
    case JSTokenKind::Class:
        return "CLASS";
    case JSTokenKind::This:
        return "THIS";
    case JSTokenKind::Extends:
        return "EXTENDS";
    case JSTokenKind::Super:
        return "SUPER";
    case JSTokenKind::Default:
        return "DEFAULT";
    case JSTokenKind::TypeOf:
        return "TYPEOF";
    case JSTokenKind::InstanceOf:
        return "INSTANCEOF";
    case JSTokenKind::Delete:
        return "DELETE";
    case JSTokenKind::New:
        return "NEW";
    case JSTokenKind::Return:
        return "RETURN";
    case JSTokenKind::Break:
        return "BREAK";
    case JSTokenKind::Continue:
        return "CONTINUE";
    case JSTokenKind::Throw:
        return "THROW";
    case JSTokenKind::If:
        return "IF";
    case JSTokenKind::Else:
        return "ELSE";
    case JSTokenKind::Switch:
        return "SWITCH";
    case JSTokenKind::Case:
        return "CASE";
    case JSTokenKind::Try:
        return "TRY";
    case JSTokenKind::Catch:
        return "CATCH";
    case JSTokenKind::Do:
        return "DO";
    case JSTokenKind::While:
        return "WHILE";
    case JSTokenKind::For:
        return "FOR";
    case JSTokenKind::In:
        return "IN";
    case JSTokenKind::Of:
        return "Of";
    case JSTokenKind::True:
        return "TRUE";
    case JSTokenKind::False:
        return "FALSE";
    case JSTokenKind::Null:
        return "NULL";
    case JSTokenKind::Void:
        return "VOID";
    case JSTokenKind::Undefined:
        return "UNDEFINED";
    case JSTokenKind::Import:
        return "IMPORT";
    case JSTokenKind::Export:
        return "EXPORT";
    case JSTokenKind::String:
        return "STRING(" + literal.getValue<JSString>() + ")";
    case JSTokenKind::Identifier:
        return "IDENTIFIER(" + literal.getValue<JSString>() + ")";
    case JSTokenKind::Numeric:
        return "NUMERIC(" + std::to_string(literal.getValue<JSNumber>()) + ")";
    case JSTokenKind::Eof:
        return "EOF";
    }

    return "UNKNOWN TOKEN";
}

}; // namespace minijsc
