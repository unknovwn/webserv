//
// Created by Gueren Drive on 4/12/21.
//

#pragma once

#include <string>
#include <map>

// HTTP response status codes -----

// Informational responses (100–199)
// Successful responses (200–299)
// Redirects (300–399)
// Client errors (400–499)
// Server errors (500–599)

// ---------------------------------

class Response {
 public:
  enum StatusCodes {
    kContinue = 100,
    kProcessing = 102,
    kOk = 200,
    kCreated = 201,
    kAccepted = 202,
    kMovedPermanently = 301,
    kBadRequest = 400,
    kForbidden = 403,
    kNotFound = 404,
    kMethodNotAllowed = 405,
    kRequestTimeout = 408,
    kLengthRequired = 411,
    kPayloadTooLarge = 413,
    kInternalServerError = 500
  };
  // Constructors, destructor, operator= ---------------------------------------
  Response() = delete;
  explicit Response(int status);
  Response(const Response& x) = default;
  ~Response() = default;
  Response& operator=(const Response& x) = default;
  // ---------------------------------------------------------------------------

  // Getters -------------------------------------------------------------------
  [[nodiscard]] const std::string& get_status_message() const;
  [[nodiscard]] const std::string& get_header_value(
                                               const std::string& header) const;
  [[nodiscard]] const std::string& get_body() const;
  // ---------------------------------------------------------------------------

  // Member-functions ----------------------------------------------------------
  void AddHeader(const std::string& header, const std::string& value);
  void AddToBody(const std::string& content);
  // ---------------------------------------------------------------------------


 private:
  int                                      status_;
  static std::map<int, std::string>        status_messages_;
  std::map<const std::string, std::string> headers_;
  std::string                              body_;
};