//
// Created by Gueren Drive on 3/30/21.
//

#include <iostream>
#include <iomanip>
#include <sstream>

#include "lexer.hpp"

// Constructors, Destructor, operator '=' --------------------------------------

Lexer::Lexer() : lexeme_(), state_(Lexer::State::kDirectiveState) {}

Lexer::Lexer(const std::string& file_name)
: lexeme_(), state_(Lexer::State::kDirectiveState) {
  this->OpenFile(file_name);
}

Lexer& Lexer::operator=(const Lexer &x) {
  if (this == &x) {
    return *this;
  }
  lexeme_ = x.lexeme_;
  state_  = x.state_;
  return *this;
}

// -----------------------------------------------------------------------------

// Getters, Setters ------------------------------------------------------------

void Lexer::set_state(const State& state) {
  state_ = state;
}

[[maybe_unused]] const std::vector<Token>& Lexer::get_lexeme() const {
  return lexeme_;
}

// -----------------------------------------------------------------------------

// Member-functions ------------------------------------------------------------


void                  Lexer::OpenFile(const std::string &file_name) {
  std::ifstream config(file_name);
  std::string   line;
  size_t        line_nb = 1;

  if (!config.is_open()) {
    throw Lexer::FileError();
  }
  std::getline(config, line);
  while (config) {
    line = DeleteComment(line);
    this->AddTokens(line, line_nb);
    std::getline(config, line);
    ++line_nb;
  }
  this->AddToken(TokenType::kEof, "EOF", line_nb);
  config.close();
}

TokenType             Lexer::WhatTokenType(const std::string& s) {
  if (s == OPEN_BLOCK) {
    return TokenType::kOpenBlock;
  } else if (s == CLOSE_BLOCK) {
    return TokenType::kCloseBlock;
  } else if (s == SEP) {
    return TokenType::kSeparator;
  } else if (state_ == Lexer::State::kDirectiveState) {
    return TokenType::kDirective;
  } else if (state_ == Lexer::State::kArgumentState) {
    return TokenType::kArgument;
  }
  return TokenType::kEof;
}

void                  Lexer::AddToken(
                                      const TokenType& token_type,
                                      const std::string& value,
                                      const size_t& line_nb
                                     ) {
  lexeme_.emplace_back(token_type, value, line_nb);
}

void                  Lexer::AddToken(const std::string& word, size_t line_nb) {
  switch (this->WhatTokenType(word)) {
    case TokenType::kOpenBlock: {
      lexeme_.emplace_back(Token(TokenType::kOpenBlock, OPEN_BLOCK, line_nb));
      state_ = Lexer::State::kDirectiveState;
      break;
    }
    case TokenType::kCloseBlock: {
      lexeme_.emplace_back(Token(TokenType::kCloseBlock, CLOSE_BLOCK, line_nb));
      state_ = Lexer::State::kDirectiveState;
      break;
    }
    case TokenType::kSeparator: {
      lexeme_.emplace_back(Token(TokenType::kSeparator, SEP, line_nb));
      state_ = Lexer::State::kDirectiveState;
      break;
    }
    case TokenType::kDirective: {
      lexeme_.emplace_back(TokenType::kDirective, word, line_nb);
      state_ = Lexer::State::kArgumentState;
      break;
    }
    default: {
      lexeme_.emplace_back(TokenType::kArgument, word, line_nb);
    }
  }
}

void                  Lexer::AddTokens(
                                       const std::string &line,
                                       size_t line_nb
                                      ) {
  std::istringstream iss(line);
  std::string        word;
  size_t             found_index;

  while (iss >> word) {
    found_index = word.find(SEP, 0);
    if (found_index != std::string::npos) {
      this->AddToken(word.substr(0, found_index), line_nb);
      this->AddToken(SEP, line_nb);
      if (found_index != word.size() - 1) {
        this->AddToken(word.substr(found_index + 1), line_nb);
      }
    } else {
      this->AddToken(word, line_nb);
    }
  }
}

std::string           Lexer::DeleteComment(std::string line) {
  size_t found_index(line.find(COMMENT, 0));

  if (found_index != std::string::npos) {
    line.resize(found_index);
  }
  return line;
}

[[maybe_unused]] void Lexer::Visualize() const {
  std::string token_type_str;
  for (const auto & i : lexeme_) {
    switch (i.get_type()) {
      case TokenType::kOpenBlock: {
        token_type_str = "kOpenBlock";
        break;
      }
      case TokenType::kCloseBlock: {
        token_type_str = "kCloseBlock";
        break;
      }
      case TokenType::kSeparator: {
        token_type_str = "kSeparator";
        break;
      }
      case TokenType::kDirective: {
        token_type_str = "kDirective";
        break;
      }
      case TokenType::kArgument: {
        token_type_str = "argument";
        break;
      }
      case TokenType::kEof: {
        token_type_str = "EOF";
        break;
      }
    }
    std::cout
    << std::setw(12)
    << token_type_str
    << " : "
    << i.get_value()
    << std::endl;
  }
}

std::string           Lexer::VisualizeToString() const {
  std::string result;
  std::string token_type_str;
  for (const auto & i : lexeme_) {
    switch (i.get_type()) {
      case TokenType::kOpenBlock: {
        token_type_str = "kOpenBlock";
        break;
      }
      case TokenType::kCloseBlock: {
        token_type_str = "kCloseBlock";
        break;
      }
      case TokenType::kSeparator: {
        token_type_str = "kSeparator";
        break;
      }
      case TokenType::kDirective: {
        token_type_str = "kDirective";
        break;
      }
      case TokenType::kArgument: {
        token_type_str = "argument";
        break;
      }
      case TokenType::kEof: {
        token_type_str = "EOF";
        break;
      }
    }
    result.append(token_type_str);
    result.append(" : ");
    result.append(i.get_value());
    result.append("\n");
  }
  return result;
}

// -----------------------------------------------------------------------------

// Exception -------------------------------------------------------------------

const char* Lexer::FileError::what() const noexcept {
  return "Error: can't open file";
}

// -----------------------------------------------------------------------------
