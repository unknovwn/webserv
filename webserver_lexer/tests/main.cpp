//
// Created by Gueren Drive on 3/30/21.
//

#include <string>
#include <sstream>

#include "gtest/gtest.h"  // Google Tests

#include "../lib_include/lexer.hpp"

// FOR TEST --------------------------------------------------------------------

// TokenType
#define T_OPEN_BLOCK "kOpenBlock"
#define T_OPEN_BLOCK_C "{"
#define T_CLOSE_BLOCK "kCloseBlock"
#define T_CLOSE_BLOCK_C "}"
#define T_SEPARATOR "kSeparator"
#define T_SEPARATOR_C ";"
#define T_DIRECTIVE "kDirective"
#define T_ARGUMENT "argument"
#define T_COLON ":"
#define T_EOF "EOF"

// Listen
#define T_LISTEN "listen"
// Arguments
#define T_HOSTPORT "host:80"
#define T_DEFAULT "default"

// Server name
#define T_SERVER_NAME "server_name"
// Arguments
#define T_DOMAIN "domain1.com"
#define T_WWWDOMAIN "www.domain1.com"

// Root
#define T_ROOT "root"
// Arguments
#define T_ROOTPATH "/var/html"

// Location
#define T_LOCATION "location"
// Arguments
#define T_LOCATIONPATH "/"

// Autoindex
#define T_AUTOINDEX "autoindex"
#define T_AUTOINDEXMODE "on"

// -----------------------------------------------------------------------------

void open_block_tockens(std::istringstream& visualization) {
  std::string word;
  visualization >> word;  // kOpenBlock
  ASSERT_STREQ(T_OPEN_BLOCK, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());

  visualization >> word;  // {
  ASSERT_STREQ(T_OPEN_BLOCK_C, word.c_str());
}
void close_block_tockens(std::istringstream& visualization) {
  std::string word;
  visualization >> word;  // kCloseBlock
  ASSERT_STREQ(T_CLOSE_BLOCK, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());

  visualization >> word;  // }
  ASSERT_STREQ(T_CLOSE_BLOCK_C, word.c_str());
}

void separator_tokens(std::istringstream& visualization) {
  std::string word;
  visualization >> word;  // kSeparator
  ASSERT_STREQ(T_SEPARATOR, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());

  visualization >> word;  // ;
  ASSERT_STREQ(T_SEPARATOR_C, word.c_str());
}

void directive_tokens(std::istringstream& visualization) {
  std::string word;

  visualization >> word;  // kDirective
  ASSERT_STREQ(T_DIRECTIVE, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());
}

void argument_tokens(std::istringstream& visualization) {
  std::string word;

  visualization >> word;  // argument
  ASSERT_STREQ(T_ARGUMENT, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());
}

void eof_tokens(std::istringstream& visualization) {
  std::string word;

  visualization >> word;  // EOF
  ASSERT_STREQ(T_EOF, word.c_str());

  visualization >> word;  // :
  ASSERT_STREQ(T_COLON, word.c_str());

  visualization >> word;  // EOF
  ASSERT_STREQ(T_EOF, word.c_str());
}

TEST(CoutTest, HandeOperator) {
  std::string        file_name("../tests/base_config.webserv");
  auto               lex = new Lexer(file_name);
  std::istringstream visualization(lex->VisualizeToString());
  std::string        word;


  open_block_tockens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // listen
  ASSERT_STREQ(T_LISTEN, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // HOST:PORT
  ASSERT_STREQ(T_HOSTPORT, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // default
  ASSERT_STREQ(T_DEFAULT, word.c_str());

  separator_tokens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // server_name
  ASSERT_STREQ(T_SERVER_NAME, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // domain.com
  ASSERT_STREQ(T_DOMAIN, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // www.domain.com
  ASSERT_STREQ(T_WWWDOMAIN, word.c_str());

  separator_tokens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // root
  ASSERT_STREQ(T_ROOT, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // /var/html
  ASSERT_STREQ(T_ROOTPATH, word.c_str());

  separator_tokens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // location
  ASSERT_STREQ(T_LOCATION, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // /
  ASSERT_STREQ(T_LOCATIONPATH, word.c_str());

  open_block_tockens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // autoindex
  ASSERT_STREQ(T_AUTOINDEX, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // /
  ASSERT_STREQ(T_AUTOINDEXMODE, word.c_str());

  separator_tokens(visualization);
  close_block_tockens(visualization);
  close_block_tockens(visualization);

  open_block_tockens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // listen
  ASSERT_STREQ(T_LISTEN, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // HOST:PORT
  ASSERT_STREQ(T_HOSTPORT, word.c_str());

  separator_tokens(visualization);
  directive_tokens(visualization);

  visualization >> word;  // root
  ASSERT_STREQ(T_ROOT, word.c_str());
  argument_tokens(visualization);
  visualization >> word;  // /var/html
  ASSERT_STREQ(T_ROOTPATH, word.c_str());

  separator_tokens(visualization);
  close_block_tockens(visualization);

  eof_tokens(visualization);

  ASSERT_EQ(29, lex->get_lexeme().size());
  delete lex;
}
