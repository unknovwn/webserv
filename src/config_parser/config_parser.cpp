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

std::vector<Server> ConfigParser::ParseConfig(
    const std::vector<Token>& tokens) {
  std::vector<Server> servers;

  tokensIt_ = tokens.begin();
  currToken_ = *tokensIt_;

  ParseRoot(servers);
  for (const auto& server : servers) {
    if (server.GetLocations().empty()) {
      throw NoLocations();
    }
  }
  return servers;
}


void ConfigParser::NextToken() {
  ++tokensIt_;
  currToken_ = *tokensIt_;
}

void ConfigParser::Expect(TokenType type) {
  if (currToken_.get_type() != type) {
    if (type == TokenType::kSeparator) {
      throw ExpectedSeparator((*(tokensIt_ - 1)).get_line_nb());
    }
    throw UnexpectedToken(currToken_.get_value(), currToken_.get_line_nb());
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
  }
  NextToken();
}

void ConfigParser::ParseLocation(Location& location) {
  while (currToken_.get_type() != TokenType::kCloseBlock) {
    Expect(TokenType::kDirective);
    ParseLocationDirective(location);
  }
}

void ConfigParser::ParseServerDirective(Server& server) {
  auto handlerIt = kServerDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kServerDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value(), currToken_.get_line_nb());
  }

  NextToken();
  Expect(TokenType::kArgument);
  auto handler = (*handlerIt).second;
  (this->*handler)(server);
  if ((*handlerIt).first == "location") {
    Expect(TokenType::kCloseBlock);
  } else {
    NextToken();
    Expect(TokenType::kSeparator);
  }
  NextToken();
}

void ConfigParser::ParseLocationDirective(Location& location) {
  auto handlerIt = kLocationDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kLocationDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value(), currToken_.get_line_nb());
  }

  NextToken();
  Expect(TokenType::kArgument);
  auto handler = (*handlerIt).second;
  (this->*handler)(location);
  NextToken();
  Expect(TokenType::kSeparator);
  NextToken();
}


void ConfigParser::ListenHandler(Server& server) {
  std::ostringstream listen;
  std::string        host;
  int                port;

  auto address    = currToken_.get_value();
  auto colonCount = std::count(address.begin(), address.end(), ':');

  if (colonCount > 1) {
    throw InvalidArgument(address, currToken_.get_line_nb());
  }

  if (colonCount == 1) {
    auto tokens = split(address, ":");
    if (tokens.size() != 2) {
      throw InvalidArgument(address, currToken_.get_line_nb());
    }
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
  if (host == "localhost") {
    host = "127.0.0.1";
  }
  if (!isHostValid(host)) {
    throw InvalidArgument(address, currToken_.get_line_nb());
  }
  if (port == -1) {
    throw InvalidArgument(address, currToken_.get_line_nb());
  }
  listen << host << ":" << port;
  server.SetListen(listen.str());
}

bool ConfigParser::isHostValid(const std::string& host) {
  auto anumbers = split(host, ".");

  if (anumbers.size() != 4) {
    return false;
  }

  for (const auto& anumber : anumbers) {
    if (!contains_only_digits(anumber)
        || (anumber.length() > 1 && anumber[0] == '0')) {
      return false;
    }
    int inumber = std::stoi(anumber);
    if (inumber > 255) {
      return false;
    }
  }
  return true;
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
  while (currToken_.get_type() == TokenType::kArgument) {
    server.AddServerName(currToken_.get_value());
    NextToken();
  }
  --tokensIt_;
}

void ConfigParser::MaxBodySizeHandler(Server& server) {
  auto arg = currToken_.get_value();
  if (!(contains_only_digits(arg))) {
    throw InvalidArgument(arg, currToken_.get_line_nb());
  }
  server.SetMaxBodySize(std::stoi(arg));
}

void ConfigParser::LocationHandler(Server& server) {
  std::string uri  = currToken_.get_value();
  int         line = currToken_.get_line_nb();

  Location location;
  location.SetUri(uri);
  NextToken();
  Expect(TokenType::kOpenBlock);
  NextToken();
  ParseLocation(location);
  try {
    server.AddLocation(location);
  } catch (Server::Exception& e) {
    throw IdenticalLocationPaths(uri, line);
  }
}


void ConfigParser::AllowedMethodsHandler(Location& location) {
  while (currToken_.get_type() == TokenType::kArgument) {
    auto method = currToken_.get_value();
    if (std::find(kHTTPMethods.begin(), kHTTPMethods.end(), method)
        == kHTTPMethods.end()) {
      throw InvalidArgument(method, currToken_.get_line_nb());
    }
    location.AddMethod(method);
    NextToken();
  }
  --tokensIt_;
}

void ConfigParser::AutoindexHandler(Location& location) {
  auto state = currToken_.get_value();
  if (state == "on") {
    location.SetAutoindex(true);
  } else if (state == "off") {
    location.SetAutoindex(false);
  } else {
    throw InvalidArgument(state, currToken_.get_line_nb());
  }
}

void ConfigParser::IndexHandler(Location& location) {
  while (currToken_.get_type() == TokenType::kArgument) {
    location.AddIndex(currToken_.get_value());
    NextToken();
  }
  --tokensIt_;
}

void ConfigParser::RootHandler(Location& location) {
  location.SetRoot(currToken_.get_value());
}

void ConfigParser::SaveDirectoryHandler(Location& location) {
  location.SetUploadDir(currToken_.get_value());
}


ConfigParser::ConfigError::ConfigError(int line) : line_(line) {}

int ConfigParser::ConfigError::GetLine() const {
  return line_;
}

ConfigParser::UnexpectedToken::UnexpectedToken(const std::string& tokenName,
    int line) : ConfigError(line), tokenName_(tokenName) {}

const std::string ConfigParser::UnexpectedToken::what() const throw() {
  std::ostringstream message;

  message << "Unexpected token: " << tokenName_;
  return message.str();
}

ConfigParser::UnknownDirective::UnknownDirective(
    const std::string& directiveName, int line)
  : ConfigError(line), directiveName_(directiveName) {}

const std::string ConfigParser::UnknownDirective::what() const throw() {
  std::ostringstream message;

  message << "Unknown directive: " << directiveName_;
  return message.str();
}

ConfigParser::ExpectedSeparator::ExpectedSeparator(int line)
  : ConfigError(line) {}

const std::string ConfigParser::ExpectedSeparator::what() const throw() {
  std::ostringstream message;

  message << "Expected semicolon";
  return message.str();
}

ConfigParser::InvalidArgument::InvalidArgument(const std::string& argument,
    int line) : ConfigError(line), argument_(argument) {}

const std::string ConfigParser::InvalidArgument::what() const throw() {
  std::ostringstream message;

  message << "Invalid argument: " << argument_;
  return message.str();
}

ConfigParser::NoLocations::NoLocations() : ConfigError(0) {}

const std::string ConfigParser::NoLocations::what() const throw() {
  return "Each server must have at least one location";
}

ConfigParser::IdenticalLocationPaths::IdenticalLocationPaths(
    const std::string& path, int line) : ConfigError(line), path_(path) {}

const std::string ConfigParser::IdenticalLocationPaths::what() const throw() {
  std::ostringstream message;

  message << "Server locations have identical paths: " << path_;
  return message.str();
}
