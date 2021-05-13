//
// Created by Gueren Drive on 5/8/21.
//

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>

#include "request.hpp"
#include "request_parser.hpp"

RequestParser::RequestParser()
: content_length_(0), state_(State::kMethodState),
  expect_body_(false), chunked_body_(false) {
  try {
    request_ = std::make_shared<Request>();
  } catch (...) {
    throw;
  }
  handlers_ = {
    {State::kMethodState,          [&] {HandleMethod();}},
    {State::kPathState,            [&] {HandlePath();}},
    {State::kProtocolVersionState, [&] {HandleProtocolVersion();}},
    {State::kHeaderState,          [&] {HandleHeader();}},
    {State::kContentLengthState,   [&] {HandleChunkedContentLength();}},
    {State::kBodyState,            [&] {HandleBody();}}
  };
  methods_handlers_ = {
    {"HEAD", [&] { HandleMethodHead(); }},
    {"GET",  [&] { HandleMethodGet(); }},
    {"PUT",  [&] { HandleMethodPut(); }},
    {"POST", [&] { HandleMethodPost(); }}
  };
}

RequestParser::RequestParser(const RequestParser &other)
: request_(other.request_),
  process_str_(other.process_str_),
  saver_(other.saver_),
  content_length_(other.content_length_),
  state_(other.state_),
  expect_body_(other.expect_body_),
  chunked_body_(other.chunked_body_)
{
  handlers_ = {
          {State::kMethodState,          [&] {HandleMethod();}},
          {State::kPathState,            [&] {HandlePath();}},
          {State::kProtocolVersionState, [&] {HandleProtocolVersion();}},
          {State::kHeaderState,          [&] {HandleHeader();}},
          {State::kContentLengthState,   [&] {HandleChunkedContentLength();}},
          {State::kBodyState,            [&] {HandleBody();}}
  };
  methods_handlers_ = {
          {"HEAD", [&] { HandleMethodHead(); }},
          {"GET",  [&] { HandleMethodGet(); }},
          {"PUT",  [&] { HandleMethodPut(); }},
          {"POST", [&] { HandleMethodPost(); }}
  };
}

RequestParser &RequestParser::operator=(const RequestParser &other) {
  if (this == &other) {
    return *this;
  }
  request_        = other.request_;
  process_str_    = other.process_str_;
  saver_          = other.saver_;
  content_length_ = other.content_length_;
  state_          = other.state_;
  expect_body_    = other.expect_body_;
  chunked_body_   = other.chunked_body_;
  return *this;
}

Request *RequestParser::ParseRequest(const std::string &request_str = "") {
  process_str_.append(request_str);
  if (state_ == State::kMethodState) {
    LeftSpaceTrim(process_str_);
  }
  while (state_ != State::kRequestIsReady && !process_str_.empty()) {
    try {
      handlers_[state_]();
    } catch (RequestParserException & e) {
      std::cerr << e.what() << std::endl;
      Reset();
      throw BadRequest();
    }
  }
  if (state_ == State::kRequestIsReady) {
    auto *request = new Request(*request_);
    Reset();
    return request;
  }
  return nullptr;
}

bool RequestParser::Empty() const {
  return process_str_.empty();
}

void RequestParser::Reset() {
  request_.reset();
  request_ = std::make_shared<Request>();
  process_str_.clear();
  saver_.clear();
  content_length_ = 0;
  state_ = State::kMethodState;
  expect_body_ = false;
  chunked_body_ = false;
}

void RequestParser::HandleMethod() {
  if (!TryTakeTokenBySeparators(" ")) {
    return;
  }
  try {
    methods_handlers_.at(saver_)();
  } catch (...) {
    PrintError(saver_ + ": unknown method");
    Reset();
    throw BadRequest();
  }
  saver_.clear();
  state_ = State::kPathState;
}

void RequestParser::HandleMethodHead() {
  request_->SetMethod("HEAD");
}

void RequestParser::HandleMethodGet() {
  request_->SetMethod("GET");
}

void RequestParser::HandleMethodPut() {
  request_->SetMethod("PUT");
  expect_body_ = true;
}

void RequestParser::HandleMethodPost() {
  request_->SetMethod("POST");
  expect_body_ = true;
}

void RequestParser::HandlePath() {
  if (!TryTakeTokenBySeparators(" ")) {
    return;
  }
  request_->SetPath(saver_);
  saver_.clear();
  LeftSpaceTrim(process_str_);
  state_ = State::kProtocolVersionState;
}

void RequestParser::HandleProtocolVersion() {
  if (!TryTakeTokenBySeparators("\n")) {
    return;
  }
  if (saver_ != "HTTP/1.1") {
    PrintError(saver_ + ": unknown version");
    Reset();
    throw BadRequest();
  }
  request_->SetAProtocol(saver_);
  saver_.clear();
  LeftSpaceTrim(process_str_);
  state_ = State::kHeaderState;
}

void RequestParser::HandleHeader() {
  if (IsSpaces(saver_) && IsHeadersOrBodyEnd()) {
    if (expect_body_) {
      state_ = chunked_body_ ? State::kContentLengthState : State::kBodyState;
    } else {
      state_ = State::kRequestIsReady;
    }
    EraseUpToSymbolInclusive(process_str_, '\n');
    return;
  }
  if (!TryTakeTokenBySeparators("\n")) {
    return;
  }
  size_t colon_index = saver_.find(':');
  if (colon_index == std::string::npos) {
    PrintError(saver_ + ": bad header format");
    Reset();
    throw BadRequest();
  }
  std::string header(saver_.substr(0, colon_index));
  std::string value(saver_.substr(colon_index + 1));

  SpaceTrim(header);
  SpaceTrim(value);
  if (!chunked_body_ && IsTransferEncoding(header, value)) {
    chunked_body_ = true;
  }
  request_->SetHeader(header, value);
  saver_.clear();
}

void RequestParser::HandleBody() {
  if (!chunked_body_ && IsHeadersOrBodyEnd()) {
    state_ = State::kRequestIsReady;
    EraseUpToSymbolInclusive(process_str_, '\n');
    return;
  }
  if (!chunked_body_ && !content_length_) {
    try {
      content_length_ = TakeContentLength();
    } catch (RequestParserException &e) {
      PrintError(e.what());
      Reset();
      throw BadRequest();
    }
  }
  if (saver_.length() + process_str_.length() < content_length_ + 2) {
    ProcessStrToSaver();
    return;
  }

  int useful_symbols = content_length_ - saver_.length();
  if (useful_symbols < 0) {
    saver_.pop_back();
  } else {
    saver_.append(process_str_.substr(0, useful_symbols));
  }

  process_str_.erase(0, useful_symbols + 2);
  request_->AddToBody(saver_);
  saver_.clear();
  content_length_ = 0;
  state_ = chunked_body_ ? State::kContentLengthState : State::kRequestIsReady;
}

void RequestParser::HandleChunkedContentLength() {
  if (!TryTakeTokenBySeparators("\n")) {
    return;
  }
  try {
    content_length_ = std::stoi(saver_, nullptr, 16);
  } catch (std::exception & e) {
    PrintError(std::string("std::execption: ") + e.what());
    PrintError(saver_ + " bad Content-Length");
    Reset();
    throw BadRequest();
  }
  saver_.clear();
  state_ = content_length_ ? State::kBodyState : State::kRequestIsReady;
  if (!content_length_ && !process_str_.empty()) {
    EraseUpToSymbolInclusive(process_str_, '\n');
  }
}

bool RequestParser::IsFake() {
  return saver_.empty()  && IsSpaces(process_str_)
         && state_ == State::kMethodState;
}

bool RequestParser::TryTakeTokenBySeparators(const std::string &separators) {
  size_t separator_index = process_str_.find_first_of(separators);

  if (separator_index == std::string::npos) {
    ProcessStrToSaver();
    return false;
  }
  saver_.append(process_str_.substr(0, separator_index));
  if (saver_.back() == '\r') {
    saver_.pop_back();
  }
  process_str_.erase(0, separator_index + 1);
  return true;
}

void RequestParser::ProcessStrToSaver() {
  saver_.append(process_str_);
  process_str_.clear();
}

bool RequestParser::IsHeadersOrBodyEnd() {
  return (process_str_.size() > 1 && process_str_[1] == '\n') ||
          (saver_.front() == '\r' && process_str_.front() == '\n');
}

int RequestParser::TakeContentLength() {
  try {
    auto length = request_->GetHeaders().at("Content-Length");
    return std::stoi(length);
  } catch (...) {
    throw RequestParserException("no Content-Length");
  }
}

// Exceptions ------------------------------------------------------------------
RequestParser::RequestParserException::RequestParserException()
: error_("RequestParser") {}

RequestParser::RequestParserException::RequestParserException(const char *error)
: error_(error) {}

const char * RequestParser::RequestParserException::what() noexcept {
  return error_;
}

const char *RequestParser::BadRequest::what() noexcept {
  return "bad request";
}
// -----------------------------------------------------------------------------

// Utils -----------------------------------------------------------------------
bool RequestParser::IsSpaces(const std::string &str) {
  return  str.empty() || (std::find_if(str.begin(), str.end(), [](char c) {
    return !std::isspace(c);
  }) == str.end());
}

void RequestParser::LeftSpaceTrim(std::string &str) {
  auto first_not_space = std::find_if(str.begin(), str.end(), [](char ch) {
    return !std::isspace(ch);
  });
  str.erase(str.begin(), first_not_space);
}

void RequestParser::RightSpaceTrim(std::string &str) {
  auto last_not_space = std::find_if(str.rbegin(), str.rend(), [](char ch) {
    return !std::isspace(ch);
  });
  // TODO(gdrive): check it
  str.erase(last_not_space.base(), str.end());
}

void RequestParser::SpaceTrim(std::string &str) {
  LeftSpaceTrim(str);
  RightSpaceTrim(str);
}

void RequestParser::PrintError(const std::string &error) {
  std::cerr << "RequestParser: " << error << std::endl;
}

bool RequestParser::IsTransferEncoding(const std::string &header,
                                       const std::string &value) {
  return header == "Transfer-Encoding" && value == "chunked";
}

void RequestParser::EraseUpToSymbolInclusive(std::string &str, char c) {
  size_t index = str.find(c);

  if (index != std::string::npos) {
    str.erase(0, index + 1);
  }
}

bool RequestParser::IsHexNumbers(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !RequestParser::IsHexNumber(ch);
  }) == s.end();
}

bool RequestParser::IsHexNumber(unsigned char c) {
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}
// -----------------------------------------------------------------------------
