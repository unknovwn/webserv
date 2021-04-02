//
// Created by Gueren Drive on 3/31/21.
//

#include "../lib_include/token.hpp"

#include <utility>

// Constructors, destructor ----------------------------------------------------

Token::Token(const TokenType& type, const size_t& line_nb)
: type_(type), value_(), line_nb_(line_nb)
{}

Token::Token(
             const TokenType& type,
             std::string  value,
             const size_t& line_nb
            )
: type_(type), value_(std::move(value)), line_nb_(line_nb)
{}

// -----------------------------------------------------------------------------

// Operators -------------------------------------------------------------------

Token& Token::operator=(const Token &x) {
  if (this == &x) {
    return *this;
  }
  type_   = x.type_;
  value_  = x.value_;
  return *this;
}

// -----------------------------------------------------------------------------

// Set, Get --------------------------------------------------------------------

const TokenType&               Token::get_type() const {
  return type_;
}

const std::string&             Token::get_value() const {
  return value_;
}

[[maybe_unused]] const size_t& Token::get_line_nb() const {
  return line_nb_;
}

// -----------------------------------------------------------------------------
