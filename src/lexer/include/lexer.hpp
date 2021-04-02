//
// Created by Gueren Drive on 3/30/21.
//

#pragma once

# include <string>
# include <fstream>
# include <exception>
# include <vector>

# include "token.hpp"

# define COMMENT     "#"
# define OPEN_BLOCK  "{"
# define CLOSE_BLOCK "}"
# define SEP         ";"

class Lexer {
 public:
  enum class State {
    kDirectiveState,
    kArgumentState
  };

  // Constructors, Destructor, operator '=' -----------------------------------
  Lexer();
  explicit Lexer(const std::string & file_name);
  Lexer(const Lexer& x) = default;
  Lexer& operator=(const Lexer& x);
  ~Lexer() = default;
  // ---------------------------------------------------------------------------

  // Getters, Setters ----------------------------------------------------------
  void set_state(const State& state);

  [[maybe_unused]] [[nodiscard]] const std::vector<Token>& get_lexeme() const;
  // ---------------------------------------------------------------------------

  // Member-functions ----------------------------------------------------------
  void               OpenFile(const std::string& file_name);
  TokenType          WhatTokenType(const std::string& s);

  void               AddToken(
                              const TokenType& token_type,
                              const std::string& value,
                              const size_t& line_nb = 0);

  void               AddToken(const std::string& word, size_t line_nb = 0);
  void               AddTokens(const std::string& line, size_t line_nb = 0);
  static std::string DeleteComment(std::string line);

  [[maybe_unused]] void     Visualize() const;
  [[nodiscard]] std::string VisualizeToString() const;
  // ---------------------------------------------------------------------------

  // Exceptions ----------------------------------------------------------------
  class FileError : public std::exception {
    [[nodiscard]] const char* what() const noexcept override;
  };
  // ---------------------------------------------------------------------------

 private:
  std::vector<Token> lexeme_;
  State         state_;
};
