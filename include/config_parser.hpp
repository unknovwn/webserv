#pragma once

#include <except>
#include <vector>
#include <map>
#include <string>

class ConfigParser {
  ConfigParser() {}

  typedef void (*serverDirectiveHandler)(Server&);
  typedef void (*localDirectiveHandler)(Location&);

  const std::map<std::string, directiveHandler>
    kServerDirectiveHandlers;
  const std::map<std::string, directiveHandler>
    kLocationDirectiveHandlers;
  const std::vector<str::string> kHTTPMethods;

  std::vector<Token>::const_iterator tokensIt_;
  Token                              currToken_;
  const char*                        kFileName_;

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

  class UnexpectedToken: public ConfigSyntaxError {
    const char* tokenName_;

   public:
    UnexpectedToken(const char* tokenName, const char* file, int line);
    const char* what() const throw() override;
  };

  class UnknownDirective: public ConfigSyntaxError {
    const char* directiveName_;

   public:
    UnknownDirective(const char* directiveName, const char* file, int line);
    const char* what() const throw() override;
  };

  class ExpectedSeparator: public ConfigSyntaxError {
   public:
    ExpectedSeparator(const char* file, int line);
    const char* what() const throw() override;
  };

  class InvalidArgument: public ConfigSyntaxError {
    const char* argument_;

   public:
    InvalidArgument(const char* argument, const char* file, int line);
    const char* what() const throw() override;
  };

 public:
  class ConfigSyntaxError: public std::exception {
    const char* file_;
    int         line_;

   protected:
    ConfigSyntaxError(const char file, int line);
   public:
    const char* GetFile() const;
    int         GetLine() const;
    const char* what() const throw() = 0;
  };

  ConfigParser(const ConfigParser&)   = delete;
  void operator=(const ConfigParser&) = delete;
  ~ConfigParser();

  static ConfigParser& GetInstance();
  std::vector<Server>  ParseConfig(const std::vector<Token>& tokens);
};
