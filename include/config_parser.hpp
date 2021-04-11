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
  std::string                        kFileName_;

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
  class ConfigSyntaxError {
    const std::string file_;
    int               line_;

   protected:
    ConfigSyntaxError(const std::string& file, int line);
   public:
    const std::string GetFile() const;
    int         GetLine() const;
    virtual const std::string what() const throw() = 0;
  };

 public:
  class UnexpectedToken: public ConfigSyntaxError {
    const std::string tokenName_;

   public:
    UnexpectedToken(const std::string& tokenName,
        const std::string& file, int line);
    const std::string what() const throw() override;
  };

  class UnknownDirective: public ConfigSyntaxError {
    const std::string directiveName_;

   public:
    UnknownDirective(const std::string& directiveName,
        const std::string& file, int line);
    const std::string what() const throw() override;
  };

  class ExpectedSeparator: public ConfigSyntaxError {
   public:
    ExpectedSeparator(const std::string& file, int line);
    const std::string what() const throw() override;
  };

  class InvalidArgument: public ConfigSyntaxError {
    const std::string argument_;

   public:
    InvalidArgument(const std::string& argument,
        const std::string& file, int line);
    const std::string what() const throw() override;
  };

 public:
  ConfigParser(const ConfigParser&)   = delete;
  void operator=(const ConfigParser&) = delete;
  ~ConfigParser() = default;

  static ConfigParser& GetInstance();
  std::vector<Server>  ParseConfig(const std::string& fileName,
      const std::vector<Token>& tokens);
};
