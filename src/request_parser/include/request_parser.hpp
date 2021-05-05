
// Created by Gueren Drive on 4/3/21.
//

#pragma once

#include <string>
#include <sstream>
#include <map>

#include "request.hpp"

// HEADERS ---------------------------------------------------------------------

// Accept-Charset
// Accept-Language
// Allow
// Authorization
// Content-Language
// Content-Length
// Content-Location
// Content-Type
// Date
// Host
// Last-Modified
// Location
// Referer
// Retry-After
// Server
// User-Agent
// WWW-Authenticate

/*
 * Accept-Charsets: utf-8, iso-8859-1;q=0.5, *;q=0.1
 *
 * The header tells what encoding the client can understand.
 * If the server cannot serve any of the provided encodings,
 * then the header is ignored.
 */

#define ACCEPT_CHARSET "Accept-Charset"

/*
 * Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5
 *
 * The header tells the server which languages the client understands
 * and which locale is preferred.
 * If the server cannot provide content in any of the languages
 * suggested in the header, then it is customary to ignore
 * the header in such cases.
 */

#define ACCEPT_LANGUAGE "Accept-Language"

/*
 * Allow: GET, POST, HEAD
 *
 * This header must be sent if the server responds with a 405 Method Not Allowed
 * status code to indicate which request methods can be used.
 * An empty Allow header indicates that the resource allows no request methods,
 * which might occur temporarily for a given resource, for example.
 */

#define ALLOW "Allow"

/*
 * Authorization: <type> <user data>
 *
 * The header includes user data to authenticate the user agent with the server
 * usually after the server responded with a 401 Unauthorized status.
 */

#define AUTHORIZATION "Authorization"

/*
 * Content-Language: de-DE, en-CA
 *
 * The title is used to describe the languages of the content available to the
 * audience, thus allowing the user to choose the language according to their
 * preferences.
 */

#define CONTENT_LANGUAGE "Content-Language"

/*
 * Content-Length: <length>
 * <length> The length in decimal number of octets.
 *
 * The Content-Length entity header indicates the size of the entity-body,
 * in bytes, sent to the recipient
 */

#define CONTENT_LENGTH "Content-Length"

/*
 * Content-Location: <url>
 *
 * The Content-Location header indicates an alternate location for the returned
 * data. The principal use is to indicate the URL of a resource transmitted as
 * the result of content negotiation.
 */

#define CONTENT_LOCATION "Content-Location"

/*
 * Content-Type: text/html; charset=UTF-8
 *
 * The Content-Type entity header is used to indicate the media type
 * of the resource.
 */

#define CONTENT_TYPE "Content-Type"

/*
 * Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
 *
 * The Date general HTTP header contains the date and time at which the message
 * was originated.
 */

#define DATE "Date"

/*
 * Host: <host>:<port>
 * Host: developer.mozilla.org
 *
 * The Host request header specifies the host and port number of the server
 * to which the request is being sent.
 */

#define HOST "Host"

/*
 * Last-Modified: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
 *
 * The Last-Modified response HTTP header contains the date and time at which
 * the origin server believes the resource was last modified.
 */

#define  LAST_MODIFIED "Last-Modified"

/*
 * Location: <url>
 * <url> A relative (to the request URL) or absolute URL.
 *
 * The Location response header indicates the URL to redirect a page to.
 * It only provides a meaning when served with a 3xx (redirection) or 201
 * (created) status response.
 */

#define LOCATION "Location"

/*
 * Referer: <url>
 * <url> An absolute or partial address of the web page making the request.
 *
 * The Referer HTTP request header contains an absolute or partial address
 * of the page making the request.
 */

#define REFERER "Referer"

/*
 * Retry-After: <http-date>
 * Retry-After: <delay-seconds>
 *
 * The Retry-After response HTTP header indicates how long the user agent
 * should wait before making a follow-up request.
 */

#define RETRY_AFTER "Retry-After"

/*
 * Server: <product>
 * <product> The name of the software or product that handled the request.
 * Usually in a format similar to User-Agent.
 *
 * The Server header describes the software used by the origin server that
 * handled the request — that is, the server that generated the response.
 */

#define SERVER "Server"


/*
 * Transfer-Encoding: chunked
 *
 * The Transfer-Encoding header specifies the form of encoding used to safely
 * transfer the payload body to the user.
 */

#define TRANSFER_ENCODING "Transfer-Encoding"

/*
 * User-Agent: <product> / <product-version> <comment>
 * User-Agent: Mozilla/5.0 (<system-information>)
 *                                  <platform> (<platform-details>) <extensions>
 *
 * The header indicates the client software and its characteristics.
 */

#define USER_AGENT "User-Agent"

/*
 * WWW-Authenticate: <type> realm=<realm>[, charset="UTF-8"]
 *
 * <type> Authentication type. A common type is "Basic".
 * IANA maintains a list of Authentication schemes.
 *
 * realm=<realm> A description of the protected area. If no realm is specified,
 * clients often display a formatted hostname instead.
 *
 * The HTTP WWW-Authenticate response header defines the authentication method
 * that should be used to gain access to a resource.
 */

#define WWW_AUTHENTICATE "WWW-Authenticate"

// -----------------------------------------------------------------------------

// Token Triggers

#define METHOD_TRIGGER ' '
#define PATH_TRIGGER ' '
#define PROTOCOL_VERSION_TRIGGER '\n'
#define HEADER_END '\n'
#define BODY_LINE_END '\n'
#define CHUNKED_CONTENT_LENGTH_TRIGGER '\n'
#define CHUNKED_CONTENT_LENGTH_BASE 16

// -----------------------------------------------------------------------------

#define PROTOCOL_VERSION "HTTP/1.1"

class RequestParser {
 public:
  enum class State {
    kMethodState,
    kPathState,
    kProtocolVersionState,
    kHeaderState,
    kBodyState,
    kRequestIsReady
  };
  enum class ChunkedState {
    kContentLengthState,
    kContentState
  };
  // Constructors, destructor, operator= ---------------------------------------
  RequestParser();
  ~RequestParser() = default;
  RequestParser(const RequestParser& x) = default;
  RequestParser& operator=(const RequestParser& x) = default;
  // ---------------------------------------------------------------------------


  // Member-functions ----------------------------------------------------------
  Request* ParseRequest();
  Request* ParseRequest(const std::string& request);
  [[nodiscard]] Request* CloneRequest() const;
  [[nodiscard]] bool     Empty() const;
  [[maybe_unused]] void Refresh();
  // ---------------------------------------------------------------------------

  // Utils ---------------------------------------------------------------------
  static void LeftSpaceTrim(std::string* s);
  static void RightSpaceTrim(std::string* s);
  [[maybe_unused]] static void SpaceTrim(std::string* s);
  static void DeleteWordWithSpaceAfter(std::string* s, size_t word_end);
  static bool IsSpaces(const std::string& s);
  static bool IsSpaceOrTab(unsigned char c);
  static bool IsHexNumbers(const std::string& s);
  static bool IsHexNumber(unsigned char c);
  static bool IsNumber(const std::string* s);
  // ---------------------------------------------------------------------------

  // Exceptions ----------------------------------------------------------------
  class BadRequest : public std::exception {
   public:
    [[nodiscard]] const char* what() const noexcept override;
  };
  // ---------------------------------------------------------------------------

 protected:
  // Request operations --------------------------------------------------------
  void CallMethodHandler(const std::string& func_key);
  void CallHeaderHandler(const std::string& func_key,
                            const std::string& value);
  void ProcessingStrToBuffer();
  void HandleMethod();
  void HandlePath();
  void HandlePath(const std::string& path);
  void HandleProtocolVersion();
  void HandleProtocolVersion(const std::string& version);
  void HandleHeader();
  void HandleHeader(const std::string& line);
  void HandleBody();
  void HandleBody(const std::string& line);
  void HandleContentLengthBody();
  void HandleChunkedBody();
  void HandleChunkedContentLength();
  // ---------------------------------------------------------------------------

  // State ---------------------------------------------------------------------
  void SetNextState();
  void SetNextChunkedState();
  bool IsHeaderTrigger();
  bool IsRequestIsReadyTrigger();
  // ---------------------------------------------------------------------------

  // Method Handlers -----------------------------------------------------------
  void HandleGet();
  void HandleHead();
  void HandlePost();
  void HandlePut();
  // ---------------------------------------------------------------------------

  // Header Handlers -----------------------------------------------------------
  void HandleAcceptCharsets(const std::string& value);
  void HandleAcceptLanguage(const std::string& value);
  void HandleAllow(const std::string& value);
  void HandleAuthorization(const std::string& value);
  void HandleContentLanguage(const std::string& value);
  void HandleContentLength(const std::string& value);
  void HandleContentLocation(const std::string& value);
  void HandleContentType(const std::string& value);
  void HandleDate(const std::string& value);
  void HandleHost(const std::string& value);
  void HandleLastModified(const std::string& value);
  void HandleLocation(const std::string& value);
  void HandleReferer(const std::string& value);
  void HandleRetryAfter(const std::string& value);
  void HandleServer(const std::string& value);
  void HandleTransferEncoding(const std::string& value);
  void HandleUserAgent(const std::string& value);
  void HandleWWWAuthenticate(const std::string& value);
  // ---------------------------------------------------------------------------

 private:
  // Maps of handlers ----------------------------------------------------------
  typedef void (RequestParser::*HandleMethodFunc)();
  std::map<std::string, HandleMethodFunc> methods_handler_;

  typedef void (RequestParser::*HandleHeaderFunc)(const std::string& value);
  std::map<std::string, HandleHeaderFunc> headers_handler_;
  // ---------------------------------------------------------------------------

  Request      request_;
  std::string  processing_str_;
  std::string  save_buffer_;
  State        state_;
  ChunkedState chunked_state_;
  int          content_length_;
  int          chunked_content_length_;
  bool         content_length_header_;
  bool         expect_body_;
  bool         chunked_;
};
