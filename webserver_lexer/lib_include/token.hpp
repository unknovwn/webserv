//
// Created by Gueren Drive on 3/31/21.
//

#pragma once

# include <string>

enum class TokenType {
  kOpenBlock,
  kCloseBlock,
  kSeparator,
  kDirective,
  kArgument,
  kEof
};

class Token {
 public:
  // Constructors, Destructor, operator '=' -----------------------------------
  explicit Token(const TokenType& type, const size_t& line_nb = 0);

  Token(
        const TokenType& type,
        std::string  value,
        const size_t& line_nb = 0);

  Token(const Token& x) = default;
  ~Token() = default;
  Token& operator=(const Token& x);
  // --------------------------------------------------------------------------

  // Getters, Setters ----------------------------------------------------------
  [[nodiscard]] const TokenType&   get_type() const;
  [[nodiscard]] const std::string& get_value() const;

  [[maybe_unused]] [[nodiscard]] const size_t& get_line_nb() const;
  // --------------------------------------------------------------------------

 private:
  TokenType   type_;
  std::string value_;
  size_t      line_nb_;
};
