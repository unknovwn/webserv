#include <sstream>
#include <algorithm>
#include "config_parser.hpp"
#include "token.hpp"
#include "utils_string.hpp"

const std::vector<std::string> ConfigParser::kHTTPMethods = {
  "GET",
  "HEAD",
  "PUT",
  "POST"
};

ConfigParser::ConfigParser() {
  kServerDirectiveHandlers["listen"]
    = &ConfigParser::ListenHandler;
  kServerDirectiveHandlers["server_name"]
    = &ConfigParser::ServerNameHandler;
  kServerDirectiveHandlers["max_body_size"]
    = &ConfigParser::MaxBodySizeHandler;
  kServerDirectiveHandlers["location"]
    = &ConfigParser::LocationHandler;

  kLocationDirectiveHandlers["allowed_methods"]
    = &ConfigParser::AllowedMethodsHandler;
  kLocationDirectiveHandlers["autoindex"]
    = &ConfigParser::AllowedMethodsHandler;
  kLocationDirectiveHandlers["index"]
    = &ConfigParser::IndexHandler;
  kLocationDirectiveHandlers["root"]
    = &ConfigParser::RootHandler;
  kLocationDirectiveHandlers["save_directory"]
    = &ConfigParser::SaveDirectoryHandler;
}

ConfigParser& ConfigParser::GetInstance() {
  static ConfigParser instance;

  return instance;
}

std::vector<Server> ConfigParser::ParseConfig(const std::string& fileName,
    const std::vector<Token>& tokens) {
  std::vector<Server> servers;

  tokensIt_ = tokens.begin();
  currToken_ = *tokensIt_;
  kFileName_ = fileName;

  ParseRoot(servers);
  return servers;
}


void ConfigParser::NextToken() {
  ++tokensIt_;
  currToken_ = *tokensIt_;
}

void ConfigParser::Expect(TokenType type) {
  if (currToken_.get_type() != type) {
    if (type == TokenType::kSeparator) {
      throw ExpectedSeparator(kFileName_, currToken_.get_line_nb());
    }
    throw UnexpectedToken(currToken_.get_value(), kFileName_,
        currToken_.get_line_nb());
  }
}

void ConfigParser::ParseRoot(std::vector<Server>& servers) {
  while (currToken_.get_type() != TokenType::kEof) {
    Expect(TokenType::kOpenBlock);
    NextToken();
    ParseServer(servers.emplace_back(Server()));
  }
}

void ConfigParser::ParseServer(Server& server) {
  while (currToken_.get_type() != TokenType::kCloseBlock) {
    Expect(TokenType::kDirective);
    ParseServerDirective(server);
    NextToken();
  }
}

void ConfigParser::ParseLocation(Location& location) {
  while (currToken_.get_type() != TokenType::kCloseBlock) {
    Expect(TokenType::kDirective);
    ParseLocationDirective(location);
    NextToken();
  }
}

void ConfigParser::ParseServerDirective(Server& server) {
  auto handlerIt = kServerDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kServerDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value(), kFileName_,
        currToken_.get_line_nb());
  }

  NextToken();
  auto handler = (*handlerIt).second;
  (this->*handler)(server);
  NextToken();
  if ((*handlerIt).first != "location") {
    Expect(TokenType::kSeparator);
  }
}

void ConfigParser::ParseLocationDirective(Location& location) {
  auto handlerIt = kLocationDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kLocationDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value(), kFileName_,
        currToken_.get_line_nb());
  }

  NextToken();
  Expect(TokenType::kArgument);
  auto handler = (*handlerIt).second;
  (this->*handler)(location);
  NextToken();
  Expect(TokenType::kSeparator);
}


void ConfigParser::ListenHandler(Server& server) {
  std::ostringstream listen;
  std::string        host;
  int                port;

  auto address    = currToken_.get_value();
  auto colonCount = std::count(address.begin(), address.end(), ':');

  if (colonCount > 1) {
    throw InvalidArgument(address, kFileName_, currToken_.get_line_nb());
  }

  if (colonCount == 1) {
    auto tokens = split(address, ":");
    host = tokens[0];
    port = ParsePort(tokens[1]);
  } else {
    if (contains_only_digits(address)) {
      host = "127.0.0.1";
      port = ParsePort(address);
    } else {
      host = address;
      port = 80;
    }
  }
  if (port == -1) {
    throw InvalidArgument(address, kFileName_, currToken_.get_line_nb());
  }
  listen << host << ":" << port;
  server.SetListen(listen.str());
}

int ConfigParser::ParsePort(const std::string& port) {
  int result;

  if ((!contains_only_digits(port))) {
    return -1;
  }
  try {
    result = std::stoi(port);
    if (result < 1 || result > 65535) {
      return -1;
    }
    return result;
  } catch (std::exception e) {
    return -1;
  }
  return -1;
}

void ConfigParser::ServerNameHandler(Server& server) {
  server.SetServName(currToken_.get_value());
}

void ConfigParser::MaxBodySizeHandler(Server& server) {
  auto arg = currToken_.get_value();
  if (!(contains_only_digits(arg))) {
    throw InvalidArgument(arg, kFileName_, currToken_.get_line_nb());
  }
  server.SetMaxBodySize(std::stoi(arg));
}

void ConfigParser::LocationHandler(Server& server) {
  Location location;
  location.SetUri(currToken_.get_value());
  NextToken();
  Expect(TokenType::kOpenBlock);
  NextToken();
  ParseLocation(location);

  server.AddLocation(location);
}


void ConfigParser::AllowedMethodsHandler(Location& location) {
  while (currToken_.get_type() == TokenType::kArgument) {
    auto method = currToken_.get_value();
    if (std::find(kHTTPMethods.begin(), kHTTPMethods.end(), method)
        == kHTTPMethods.end()) {
      throw InvalidArgument(method, kFileName_,
          currToken_.get_line_nb());
    }
    location.AddMethod(method);
    NextToken();
  }
}

void ConfigParser::AutoindexHandler(Location& location) {
  auto state = currToken_.get_value();
  if (state == "on") {
    location.SetAutoindex(true);
  } else if (state == "off") {
    location.SetAutoindex(false);
  } else {
    throw InvalidArgument(state, kFileName_,
        currToken_.get_line_nb());
  }
}

void ConfigParser::IndexHandler(Location& location) {
  while (currToken_.get_type() == TokenType::kArgument) {
    location.AddIndex(currToken_.get_value());
    NextToken();
  }
}

void ConfigParser::RootHandler(Location& location) {
  location.SetRoot(currToken_.get_value());
}

void ConfigParser::SaveDirectoryHandler(Location& location) {
  location.SetUploadDir(currToken_.get_value());
}


ConfigParser::ConfigSyntaxError::ConfigSyntaxError(const std::string& file, int line)
  : file_(file), line_(line) {}

const std::string ConfigParser::ConfigSyntaxError::GetFile() const {
  return file_;
}

int ConfigParser::ConfigSyntaxError::GetLine() const {
  return line_;
}

ConfigParser::UnexpectedToken::UnexpectedToken(const std::string& tokenName,
    const std::string& file, int line)
  : ConfigSyntaxError(file, line), tokenName_(tokenName) {}

const std::string ConfigParser::UnexpectedToken::what() const throw() {
  std::ostringstream message;

  message << "Unexpected token: " << tokenName_;
  return message.str();
}

ConfigParser::UnknownDirective::UnknownDirective(const std::string& directiveName,
    const std::string& file, int line)
  : ConfigSyntaxError(file, line), directiveName_(directiveName) {}

const std::string ConfigParser::UnknownDirective::what() const throw() {
  std::ostringstream message;

  message << "Unknown directive: " << directiveName_;
  return message.str();
}

ConfigParser::ExpectedSeparator::ExpectedSeparator(const std::string& file,
    int line) : ConfigSyntaxError(file, line) {}

const std::string ConfigParser::ExpectedSeparator::what() const throw() {
  std::ostringstream message;

  message << "Expected semicolon";
  return message.str();
}

ConfigParser::InvalidArgument::InvalidArgument(const std::string& argument,
    const std::string& file, int line)
  : ConfigSyntaxError(file, line), argument_(argument) {}

const std::string ConfigParser::InvalidArgument::what() const throw() {
  std::ostringstream message;

  message << "Invalid argument: " << argument_;
  return message.str();
}
