#pragma once

#include <exception>
#include <vector>
#include <map>
#include <string>
#include "server.hpp"
#include "location.hpp"
#include "token.hpp"

class ConfigParser {
  ConfigParser();

  std::map<std::string, void (ConfigParser::*)(Server&)>
    kServerDirectiveHandlers;
  std::map<std::string, void (ConfigParser::*)(Location&)>
    kLocationDirectiveHandlers;

  static const std::vector<std::string> kHTTPMethods;

  std::vector<Token>::const_iterator tokensIt_;
  Token                              currToken_;

  void NextToken();
  void Expect(TokenType type);

  void ParseRoot(std::vector<Server>& servers);
  void ParseServer(Server& server);
  void ParseLocation(Location& location);
  void ParseServerDirective(Server& server);
  void ParseLocationDirective(Location& location);

  void ListenHandler(Server& server);
  int  ParsePort(const std::string& port);
  void ServerNameHandler(Server& server);
  void MaxBodySizeHandler(Server& server);
  void LocationHandler(Server& server);

  void AllowedMethodsHandler(Location& location);
  void AutoindexHandler(Location& location);
  void IndexHandler(Location& location);
  void RootHandler(Location& location);
  void SaveDirectoryHandler(Location& location);

 public:
  class ConfigError {
    int line_;

   protected:
    ConfigError(int line);
   public:
    int GetLine() const;
    virtual const std::string what() const throw() = 0;
  };

 private:
  class UnexpectedToken: public ConfigError {
    const std::string tokenName_;

   public:
    UnexpectedToken(const std::string& tokenName, int line);
    const std::string what() const throw() override;
  };

  class UnknownDirective: public ConfigError {
    const std::string directiveName_;

   public:
    UnknownDirective(const std::string& directiveName, int line);
    const std::string what() const throw() override;
  };

  class ExpectedSeparator: public ConfigError {
   public:
    ExpectedSeparator(int line);
    const std::string what() const throw() override;
  };

  class InvalidArgument: public ConfigError {
    const std::string argument_;

   public:
    InvalidArgument(const std::string& argument, int line);
    const std::string what() const throw() override;
  };

  class NoLocations: public ConfigError {
   public:
    NoLocations();
    const std::string what() const throw() override;
  };

  class IdenticalLocationPaths: public ConfigError {
    const std::string path_;

   public:
    IdenticalLocationPaths(const std::string& path, int line);
    const std::string what() const throw() override;
  };

 public:
  ConfigParser(const ConfigParser&)   = delete;
  void operator=(const ConfigParser&) = delete;
  ~ConfigParser()                     = default;

  static ConfigParser& GetInstance();
  std::vector<Server>  ParseConfig(const std::vector<Token>& tokens);
};
