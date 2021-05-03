//
// Created by Gueren Drive on 4/12/21.
//

#include <iostream>

#include "response.hpp"

// initialization static fields ------------------------------------------------

std::map<int, std::string> Response::status_messages_ = {
        {Response::StatusCodes::kContinue,         "100 Continue"},
        {Response::StatusCodes::kProcessing,       "102 Processing"},
        {Response::StatusCodes::kOk,               "200 OK"},
        {Response::StatusCodes::kCreated,          "201 Created"},
        {Response::StatusCodes::kAccepted,         "202 Accepted"},
        {Response::StatusCodes::kNoContent,        "204 No Content"},
        {Response::StatusCodes::kMovedPermanently, "301 Moved Permanently"},
        {Response::StatusCodes::kBadRequest,       "400 Bad Request"},
        {Response::StatusCodes::kForbidden,        "403 Forbidden"},
        {Response::StatusCodes::kNotFound,         "404 Not Found"},
        {Response::StatusCodes::kMethodNotAllowed, "405 Method Not Allowed"},
        {Response::StatusCodes::kRequestTimeout,   "408 Request Timeout"},
        {Response::StatusCodes::kLengthRequired,   "411 Length Required"},
        {Response::StatusCodes::kPayloadTooLarge,  "413 Payload Too Large"},
        {Response::StatusCodes::kInternalServerError,
                                                   "500 Internal Server Error"}
};

// -----------------------------------------------------------------------------

// Constructors, destructor, operator= -----------------------------------------

Response::Response(int status) : status_(status) {}

// -----------------------------------------------------------------------------

// Getters -------------------------------------------------------------------

const std::string& Response::get_status_message() const {
  return status_messages_[status_];
}

const std::string& Response::get_header_value(const std::string& header) const {
  return headers_.at(header);
}

const std::string& Response::get_body() const {
  return body_;
}

const std::map<const std::string, std::string> &Response::get_headers() const {
  return headers_;
}

// -----------------------------------------------------------------------------

// Member-functions ----------------------------------------------------------

void Response::AddHeader(const std::string &header, const std::string &value) {
  headers_[header] = value;
}

void Response::AddToBody(const std::string& content) {
  body_.append(content);
}

// -----------------------------------------------------------------------------

std::ostream &operator<<(std::ostream &out, const Response &response) {
  out
  << "\nResponse ========\n"
  << "\n--- Headers ---\n";
  if (response.get_headers().count("Host")) {
    out
    << "Host: "
    << response.get_header_value("Host") << "\n";
  }
  if (response.get_headers().count("Content-Length")) {
    out
    << "Content-Length: "
    << response.get_header_value("Content-Length") << "\n";
  }
  if (response.get_headers().count("Content-Type")) {
    out
    << "Content-Type: "
    << response.get_header_value("Content-Type") << "\n";
  }
  if (response.get_headers().count("Content-Location")) {
    out
    << "Content-Location: "
    << response.get_header_value("Content-Location") << "\n";
  }
  out
  << "---------------\n"
  << "\n-----BODY -----\n"
  << response.get_body()
  << "\n---------------\n"
  << "\n=================\n";
  return out;
}
