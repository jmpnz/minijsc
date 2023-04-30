#include "Token.h"

namespace minijsc {

// Return a string representation of the token based on its kind.
auto Token::toString() const -> std::string {
  switch (kind) {
  case TokenKind::LParen:
    return "(";
  case TokenKind::RParen:
    return ")";
  case TokenKind::LBrace:
    return "{";
  case TokenKind::RBrace:
    return "}";
  case TokenKind::LBracket:
    return "[";
  case TokenKind::RBracket:
    return "]";
  case TokenKind::Bang:
    return "!";
  case TokenKind::BangEqual:
    return "!=";
  case TokenKind::Comma:
    return ",";
  case TokenKind::Dot:
    return ".";
  case TokenKind::Minus:
    return "-";
  case TokenKind::Plus:
    return "+";
  case TokenKind::Semicolon:
    return ";";
  case TokenKind::Slash:
    return "/";
  case TokenKind::Star:
    return "*";
  case TokenKind::Equal:
    return "=";
  case TokenKind::EqualEqual:
    return "==";
  case TokenKind::Less:
    return "<";
  case TokenKind::LessEqual:
    return "<=";
  case TokenKind::GreaterEqual:
    return ">=";
  case TokenKind::Greater:
    return ">";
  case TokenKind::Let:
    return "LET";
  case TokenKind::Var:
    return "VAR";
  case TokenKind::Const:
    return "CONST";
  case TokenKind::Function:
    return "FUNCTION";
  case TokenKind::Class:
    return "CLASS";
  case TokenKind::This:
    return "THIS";
  case TokenKind::Extends:
    return "EXTENDS";
  case TokenKind::Super:
    return "SUPER";
  case TokenKind::Default:
    return "DEFAULT";
  case TokenKind::TypeOf:
    return "TYPEOF";
  case TokenKind::InstanceOf:
    return "INSTANCEOF";
  case TokenKind::Delete:
    return "DELETE";
  case TokenKind::New:
    return "NEW";
  case TokenKind::Return:
    return "RETURN";
  case TokenKind::Break:
    return "BREAK";
  case TokenKind::Continue:
    return "CONTINUE";
  case TokenKind::Throw:
    return "THROW";
  case TokenKind::If:
    return "IF";
  case TokenKind::Else:
    return "ELSE";
  case TokenKind::Switch:
    return "SWITCH";
  case TokenKind::Case:
    return "CASE";
  case TokenKind::Try:
    return "TRY";
  case TokenKind::Catch:
    return "CATCH";
  case TokenKind::Do:
    return "DO";
  case TokenKind::While:
    return "WHILE";
  case TokenKind::For:
    return "FOR";
  case TokenKind::In:
    return "IN";
  case TokenKind::Of:
    return "Of";
  case TokenKind::True:
    return "TRUE";
  case TokenKind::False:
    return "FALSE";
  case TokenKind::Null:
    return "NULL";
  case TokenKind::Import:
    return "IMPORT";
  case TokenKind::Export:
    return "EXPORT";
  case TokenKind::String:
    return "STRING(" + std::get<std::string>(literal) + ")";
  case TokenKind::Identifier:
    return "IDENTIFIER(" + std::get<std::string>(literal) + ")";
  case TokenKind::Numeric:
    return "NUMERIC(" + std::to_string(std::get<double>(literal)) + ")";
  case TokenKind::Eof:
    return "EOF";
  }

  return "UNKNOWN TOKEN";
}

}; // namespace minijsc
