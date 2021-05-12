//
// Created by Gueren Drive on 5/8/21.
//

#pragma once

class Request;

#include <string>
#include <memory>
#include <map>
#include <functional>

class RequestParser {
 public:
  RequestParser();
  RequestParser(const RequestParser &other);
  RequestParser &operator=(const RequestParser &other);

  Request *ParseRequest(const std::string &request_str);
  bool    Empty() const;
  void    Reset();

  // Exceptions ----------------------------------------------------------------
  class RequestParserException {
   public:
    RequestParserException();
    explicit RequestParserException(const char* error);
    virtual const char *what() noexcept;

   private:
    const char *error_;
  };
  class BadRequest : RequestParserException {
    const char * what() noexcept override;
  };
  // ---------------------------------------------------------------------------

 private:
  enum class State {
    kMethodState,
    kPathState,
    kProtocolVersionState,
    kHeaderState,
    kContentLengthState,
    kBodyState,
    kRequestIsReady
  };

 private:
  void HandleMethod();
  void HandleMethodHead();
  void HandleMethodGet();
  void HandleMethodPut();
  void HandleMethodPost();
  void HandlePath();
  void HandleProtocolVersion();
  void HandleHeader();
  void HandleBody();
  void HandleChunkedContentLength();

  bool IsFake();
  bool TryTakeTokenBySeparators(const std::string &separators);
  void ProcessStrToSaver();
  bool IsHeadersOrBodyEnd();
  int  TakeContentLength();

  // Utils ---------------------------------------------------------------------
  static bool IsSpaces(const std::string &str);
  static void LeftSpaceTrim(std::string &str);
  static void RightSpaceTrim(std::string &str);
  static void SpaceTrim(std::string &str);
  static void PrintError(const std::string &error);
  static bool IsTransferEncoding(const std::string &header,
                                 const std::string &value);
  static void EraseUpToSymbolInclusive(std::string &str, char c);
  static bool IsHexNumbers(const std::string &s);
  static bool IsHexNumber(unsigned char c);
  // ---------------------------------------------------------------------------

 private:
  std::map<State, std::function<void(void)> >         handlers_;
  std::map<std::string, std::function<void(void)> >   methods_handlers_;
  std::shared_ptr<Request> request_;
  std::string              process_str_;
  std::string              saver_;
  int                      content_length_;
  State                    state_;
  bool                     expect_body_;
  bool                     chunked_body_;
};
