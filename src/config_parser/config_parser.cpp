#include <ostringstream>
#include <algorithm>
#include "config_parser.hpp"
#include "utils_string.hpp"

ConfigParser::ConfigParser() {
  kServerDirectiveHandlers = {
    { "listen", &ConfigParser::ListenHandler },
    { "server_name", &ConfigParser::ServerNameHandler },
    { "max_body_size", &ConfigParser::MaxBodySizeHandler },
    { "location", &ConfigParser::LocationHandler }
  };

  kLocationDirectiveHandlers = {
    { "allowed_methods", &ConfigParser::AllowedMethodsHandler },
    { "autoindex", &ConfigParser::AutoindexHandler },
    { "index", &ConfigParser::IndexHandler },
    { "root", &ConfigParser::RootHandler },
    { "save_directory", &ConfigParser::SaveDirectoryHandler }
  };

  kHTTPMethods = { "GET", "HEAD", "PUT", "POST" };
}

static ConfigParser& ConfigParser::GetInstance() {
  static ConfigParser instance;

  return instance;
}

std::vector<Server> ConfigParser::ParseConfig(const char* fileName,
    const std::vector<Token>& tokens) {
  std::vector<Server> servers;

  tokensIt_ = tokens.begin();
  currToken_ = *tokensIt;
  fileName_ = fileName;

  ParseRoot(servers);
}


void ConfigParser::NextToken() {
  ++tokensIt_;
  curToken_ = *tokensIt_;
}

void ConfigParser::Expect(TokenType type) {
  if (currToken_.get_type() != type) {
    if (type == kSeparator) {
      throw ExpectedSeparator(kFileName_, currToken_.get_line());
    }
    throw UnexpectedToken(currToken_.get_value().c_str(), kFileName_,
        currToken_.get_line());
  }
}

void ConfigParser::ParseRoot(std::vector<Server>& servers) {
  while (currToken_.get_type() != kEof) {
    Expect(kOpenBlock);
    NextToken();
    ParseServer(servers.emplace_back(Server()));
  }
}

void ConfigParser::ParseServer(Server& server) {
  while (currToken_.get_type() != kCloseBlock) {
    Expect(kDirective);
    ParseServerDirective();
    NextToken();
  }
}

void ConfigParser::ParseLocation(Location& location) {
  while (currToken_.get_type() != kCloseBlock) {
    Expect(kDirective);
    ParseLocationDirective();
    NextToken();
  }
}

void ConfigParser::ParseServerDirective(Server& server) {
  auto handlerIt = kServerDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kServerDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value().c_str(), kFileName_,
        currToken_.get_line());
  }

  NextToken();
  (*handlerIt).second(server);
  NextToken();
  if ((*handlerIt).first != "location") {
    Expect(kSeparator);
  }
}

void ConfigParser::ParseLocationDirective(Location& location) {
  auto handlerIt = kLocationDirectiveHandlers.find(currToken_.get_value());

  if (handlerIt == kLocationDirectiveHandlers.end()) {
    throw UnknownDirective(currToken_.get_value().c_str(), kFileName_,
        currToken_.get_line());
  }

  NextToken();
  Expect(kArgument);
  (*handlerIt).second(location);
  NextToken();
  Expect(kSeparator);
}


void ConfigParser::ListenHandler(Server& server) {
  std::ostringstream listen;
  std::string        host;
  int                port;

  auto address    = currToken_.get_value();
  auto colonCount = std::count(address.begin(), address.end(), ':');

  if (colonCount > 1) {
    throw InvalidArgument(address.c_str(), kFileName_, currToken_.get_line());
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
    throw InvalidArgument(address.c_str(), kFileName_, currToken_.get_line());
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
    if (portValue < 1 || portValue > 65535) {
      return -1;
    }
    return result;
  } catch (std::exception e) {
    return -1;
  }
  return -1;
}

void ConfigParser::ServerNameHandler(Server& server) {
  server.setServerName(currToken_.get_value());
}

void ConfigParser::MaxBodySizeHandler(Server& server) {
  auto arg = currToken_.get_value();
  if (!(contains_only_digits(arg))) {
    throw InvalidArgument(arg.c_str(), kFileName_, currToken_.get_line());
  }
  server.setMaxBodySize(std::stoi(arg));
}

void ConfigParser::LocationHandler(Server& server) {
  Location location;
  location.setUri(currToken_.get_value());
  NextToken();
  Expect(kOpenBlock);
  NextToken();
  ParseLocation(location);

  server.AddLocation(location)
}


void ConfigParser::AllowedMethodsHandler(Location& location) {
  while (currToken_.get_type() == kArgument) {
    auto method = currToken_.get_value();
    if (std::find(kHTTPMethods.begin(), kHTTPMethods.end(), method)
        == kHTTPMethods.end()) {
      throw InvalidArgument(method.c_str(), kFileName_,
          currToken_.get_line());
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
    throw InvalidArgument(state.c_str(), kFileName_,
        currToken_.get_line());
  }
}

void ConfigParser::IndexHandler(Location& location) {
  while (currToken_.get_type() == kArgument) {
    location.AddIndex(currToken_.get_value());
    NextToken();
  }
}

void ConfigParser::RootHandler(Location& location) {
  location.SetRoot(currToken_.get_value());
}

void SaveDirectoryHandler() {
  location.SetUploadDir(currToken_.get_value());
}


ConfigParser::ConfigSyntaxError::ConfigSyntaxError(const char* file, int line) {
  file_ = file;
  line_ = line;
}

const char* ConfigParser::ConfigSyntaxError::GetFile() const {
  return file_;
}

int ConfigParser::ConfigSyntaxError::GetLine() const {
  return line_;
}

ConfigParser::UnexpectedToken::UnexpectedToken(const char* tokenName,
    const char* file, int line) : ConfigSyntaxError(file, line) {
  tokenName_ = tokenName;
}

const char* ConfigParser::UnexpectedToken::what() const throw() {
  std::ostringstream message;

  message << "Unexpected token: " << tokenName_ << std::endl;
  return message.str().c_str();
}

ConfigParser::UnknownDirective::UnknownDirective(const char* directiveName,
    const char* file, int line) : ConfigSyntaxError(file, line) {
  directiveName_ = directiveName;
}

const char* ConfigParser::UnknownDirective::what() const throw() {
  std::ostringstream message;

  message << "Unknown directive: " << directiveName_ << std::endl;
  return message.str().c_str();
}

ConfigParser::ExpectedSeparator::ExpectedSeparator(const char* file,
    int line) : ConfigSyntaxError(file, line) {}

const char* ConfigParser::ExpectedSeparator::what() const throw() {
  std::ostringstream message;

  message << "Expected semicolon" << std::endl;
  return message.str().c_str();
}

ConfigParser::InvalidArgument::InvalidArgument(const char* argument,
    const char* file, int line) : ConfigSyntaxError(file, line) {
  argument_ = argument;
}

const char* ConfigParser::InvalidArgument::what() const throw() {
  std::ostringstream message;

  message << "Invalid argument: " << argument_ << std::endl;
  return message.str().c_str();
}
