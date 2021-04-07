//
// Created by Gueren Drive on 3/31/21.
//

#include "token.hpp"

#include <utility>

// Constructors, destructor ----------------------------------------------------

Token::Token() {}

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
