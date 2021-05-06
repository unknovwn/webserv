//
// Created by Gueren Drive on 4/3/21.
//

#include "request_parser.hpp"

#include <string>
#include <utility>    // std::make_pair();
#include <algorithm>  // std::remove_if();

// Constructors, destructor, operator= -----------------------------------------

RequestParser::RequestParser()
: request_(), processing_str_(), save_buffer_(), state_(State::kMethodState),
chunked_state_(ChunkedState::kContentLengthState), content_length_(),
chunked_content_length_(), content_length_header_(false), expect_body_(false),
chunked_(false) {
  //  Initialization methods_handler_
  methods_handler_.insert(std::make_pair("GET", &RequestParser::HandleGet));
  methods_handler_.insert(std::make_pair("HEAD", &RequestParser::HandleHead));
  methods_handler_.insert(std::make_pair("POST", &RequestParser::HandlePost));
  methods_handler_.insert(std::make_pair("PUT", &RequestParser::HandlePut));

  //  Initialization headers_handler_
  headers_handler_.insert(std::make_pair(ACCEPT_CHARSET,
                          &RequestParser::HandleAcceptCharsets));
  headers_handler_.insert(std::make_pair(ACCEPT_LANGUAGE,
                          &RequestParser::HandleAcceptLanguage));
  headers_handler_.insert(std::make_pair(ALLOW,
                          &RequestParser::HandleAllow));
  headers_handler_.insert(std::make_pair(AUTHORIZATION,
                                         &RequestParser::HandleAuthorization));

  headers_handler_.insert(std::make_pair(CONTENT_LANGUAGE,
                          &RequestParser::HandleContentLanguage));

  headers_handler_.insert(std::make_pair(CONTENT_LENGTH,
                                         &RequestParser::HandleContentLength));
  headers_handler_.insert(std::make_pair(CONTENT_LOCATION,
                          &RequestParser::HandleContentLocation));
  headers_handler_.insert(std::make_pair(CONTENT_TYPE,
                                         &RequestParser::HandleContentType));
  headers_handler_.insert(std::make_pair(DATE,
                                         &RequestParser::HandleDate));
  headers_handler_.insert(std::make_pair(HOST,
                                         &RequestParser::HandleHost));
  headers_handler_.insert(std::make_pair(LAST_MODIFIED,
                                         &RequestParser::HandleLastModified));
  headers_handler_.insert(std::make_pair(LOCATION,
                                         &RequestParser::HandleLocation));
  headers_handler_.insert(std::make_pair(REFERER,
                                         &RequestParser::HandleReferer));
  headers_handler_.insert(std::make_pair(RETRY_AFTER,
                                         &RequestParser::HandleRetryAfter));
  headers_handler_.insert(std::make_pair(SERVER,
                                         &RequestParser::HandleServer));
  headers_handler_.insert(std::make_pair(TRANSFER_ENCODING,
                          &RequestParser::HandleTransferEncoding));
  headers_handler_.insert(std::make_pair(USER_AGENT,
                                         &RequestParser::HandleUserAgent));
  headers_handler_.insert(std::make_pair(WWW_AUTHENTICATE,
                           &RequestParser::HandleWWWAuthenticate));
}

// -----------------------------------------------------------------------------

// Member-functions ------------------------------------------------------------

Request* RequestParser::ParseRequest() {
  return this->ParseRequest("");
}

Request* RequestParser::ParseRequest(const std::string& request) {
  if (state_ == State::kMethodState && save_buffer_.empty() && (RequestParser::IsSpaces(request) || request.empty())) {
    if (processing_str_.size() == 1 && processing_str_.back() == '\n') {
      processing_str_.clear();
    }
    return nullptr;
  }
  processing_str_.append(request);
  if (save_buffer_.empty() &&
      (state_ == State::kPathState ||
      state_ == State::kProtocolVersionState)) {
    RequestParser::LeftSpaceTrim(&processing_str_);
  }
  while (state_ != State::kRequestIsReady && !processing_str_.empty()) {
    if (this->IsHeaderTrigger() || this->IsRequestIsReadyTrigger()) {
      if (processing_str_[0] == '\r') {
        processing_str_.erase(0, 2);
      } else {
        processing_str_.erase(0, 1);
      }
      this->SetNextState();
    }
    switch (state_) {
      case State::kMethodState: {
        this->HandleMethod();
        break;
      }
      case State::kPathState: {
        this->HandlePath();
        break;
      }
      case State::kProtocolVersionState: {
        this->HandleProtocolVersion();
        break;
      }
      case State::kHeaderState: {
        this->HandleHeader();
        break;
      }
      case State::kBodyState: {
        if (chunked_) {
          this->HandleChunkedBody();
          break;
        }
        if (content_length_header_) {
          this->HandleContentLengthBody();
          break;
        }
        this->HandleBody();
        break;
      }
      default: {
        break;
      }
    }
  }
  if (state_ == State::kRequestIsReady) {
    if (request_.GetHeaders().find(HOST) == request_.GetHeaders().end()) {
      throw BadRequest();
    }
    Request* save = this->CloneRequest();
    this->Refresh();
    return save;
  }
  return nullptr;
}

Request* RequestParser::CloneRequest() const {
  return new Request(request_);
}

bool     RequestParser::Empty() const {
  return processing_str_.empty();
}

[[maybe_unused]] void RequestParser::Refresh() {
  save_buffer_.clear();
  state_                  = State::kMethodState;
  expect_body_            = false;
  chunked_                = false;
  chunked_state_          = ChunkedState::kContentLengthState;
  chunked_content_length_ = 0;
  request_.Clear();
}

// -----------------------------------------------------------------------------

// Utils -----------------------------------------------------------------------

void RequestParser::LeftSpaceTrim(std::string* s) {
  s->erase(s->begin(), std::find_if(s->begin(), s->end(), [](unsigned char ch) {
    return !RequestParser::IsSpaceOrTab(ch);
  }));
}

void RequestParser::RightSpaceTrim(std::string* s) {
  s->erase(std::find_if(s->rbegin(), s->rend(), [](unsigned char ch) {
    return !RequestParser::IsSpaceOrTab(ch);
  }).base(), s->end());
}

[[maybe_unused]] void RequestParser::SpaceTrim(std::string* s) {
  RequestParser::LeftSpaceTrim(s);
  RequestParser::RightSpaceTrim(s);
}

void RequestParser::DeleteWordWithSpaceAfter(std::string* s, size_t word_end) {
  s->erase(0, word_end + 1);
  RequestParser::LeftSpaceTrim(s);
}

bool RequestParser::IsSpaces(const std::string& s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char ch) {
    return !std::isspace(ch);
  }) == s.end();
}

bool RequestParser::IsSpaceOrTab(unsigned char c) {
  return (c == ' ' || c == '\t' || c == '\v' || c == '\r');
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

bool RequestParser::IsNumber(const std::string* s) {
  return !s->empty() && std::find_if(s->begin(), s->end(), [](unsigned char c) {
    return !std::isdigit(c);
  }) == s->end();
}

// -----------------------------------------------------------------------------

// Exceptions ----------------------------------------------------------------

const char* RequestParser::BadRequest::what() const noexcept {
  return "bad request";
}

// -----------------------------------------------------------------------------

// Request operations --------------------------------------------------------

void RequestParser::CallMethodHandler(const std::string &func_key) {
  auto func_pos(methods_handler_.find(func_key));

  if (func_pos == methods_handler_.end()) {
    throw BadRequest();
  }
  auto f = (*func_pos).second;
  (this->*f)();
}

void     RequestParser::CallHeaderHandler(const std::string &func_key,
                                          const std::string& value) {
  auto func_pos(headers_handler_.find(func_key));
  if (func_pos == headers_handler_.end()) {
    return;
  }
  auto f = (*func_pos).second;
  (this->*f)(value);
}

void     RequestParser::ProcessingStrToBuffer() {
  save_buffer_.append(processing_str_);
  processing_str_.clear();
}

void     RequestParser::HandleMethod() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(METHOD_TRIGGER);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    if (save_buffer_.find('\n') != std::string::npos) {
      throw BadRequest();
    }
    return;
  }
  word = processing_str_.substr(0, found_index);
  save_buffer_.append(word);
  this->CallMethodHandler(save_buffer_);
  this->SetNextState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandlePath() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(PATH_TRIGGER);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  save_buffer_.append(word);
  this->HandlePath(save_buffer_);
  this->SetNextState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandlePath(const std::string& path) {
  request_.SetPath(path);
}

void     RequestParser::HandleProtocolVersion() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(PROTOCOL_VERSION_TRIGGER);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  RequestParser::RightSpaceTrim(&word);
  save_buffer_.append(word);
  this->HandleProtocolVersion(save_buffer_);
  this->SetNextState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandleProtocolVersion(const std::string& version) {
  if (version != PROTOCOL_VERSION) {
    throw BadRequest();
  }
  request_.SetAProtocol(version);
}

void     RequestParser::HandleHeader() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(HEADER_END);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  RequestParser::RightSpaceTrim(&word);
  save_buffer_.append(word);
  this->HandleHeader(save_buffer_);
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandleHeader(const std::string& line) {
  std::string header;
  std::string value;
  size_t      found_index;

  found_index = line.find(':');
  header = line.substr(0, found_index);
  value  = line.substr(found_index + 1);
  RequestParser::SpaceTrim(&value);
  this->CallHeaderHandler(header, value);
}

void     RequestParser::HandleBody() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(BODY_LINE_END);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  save_buffer_.append(word);
  save_buffer_.append("\n");
  this->HandleBody(save_buffer_);
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandleBody(const std::string& line) {
  request_.AddToBody(line);
}

void     RequestParser::HandleContentLengthBody() {
  std::string word;
  size_t      content_end_index;

  if (save_buffer_.size() + processing_str_.size() < content_length_) {
    this->ProcessingStrToBuffer();
    return;
  }
  content_end_index = content_length_ - save_buffer_.size();
  word = processing_str_.substr(0, content_end_index);
  save_buffer_.append(word);
  this->HandleBody(save_buffer_);
  this->SetNextState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_,
                                          content_end_index - 1);
  save_buffer_.append(processing_str_);
}

void     RequestParser::HandleChunkedBody() {
  if (chunked_state_ == ChunkedState::kContentLengthState) {
    this->HandleChunkedContentLength();
    return;
  }
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(BODY_LINE_END);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  if (word.back() == '\r') {
    word.pop_back();
  }
  save_buffer_.append(word);
  if (save_buffer_.size() != chunked_content_length_) {
    throw BadRequest();
  }
  save_buffer_.append("\n");
  this->HandleBody(save_buffer_);
  this->SetNextChunkedState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

void     RequestParser::HandleChunkedContentLength() {
  std::string word;
  size_t      found_index;

  found_index = processing_str_.find(CHUNKED_CONTENT_LENGTH_TRIGGER);
  if (found_index == std::string::npos) {
    this->ProcessingStrToBuffer();
    return;
  }
  word = processing_str_.substr(0, found_index);
  if (word.back() == '\r') {
    word.pop_back();
  }
  save_buffer_.append(word);
  if (!RequestParser::IsHexNumbers(save_buffer_)) {
    throw BadRequest();
  }
  chunked_content_length_ = std::stoi(save_buffer_, nullptr,
                                      CHUNKED_CONTENT_LENGTH_BASE);
  if (chunked_content_length_ == 0) {
    this->SetNextState();
  }
  this->SetNextChunkedState();
  RequestParser::DeleteWordWithSpaceAfter(&processing_str_, found_index);
  save_buffer_.clear();
}

// -----------------------------------------------------------------------------

// State ---------------------------------------------------------------------

void RequestParser::SetNextState() {
  switch (state_) {
    case State::kMethodState: {
      state_ = State::kPathState;
      break;
    }
    case State::kPathState: {
      state_ = State::kProtocolVersionState;
      break;
    }
    case State::kProtocolVersionState: {
      state_ = State::kHeaderState;
      break;
    }
    case State::kHeaderState: {
      if (expect_body_) {
        state_ = State::kBodyState;
        break;
      }
      state_ = State::kRequestIsReady;
      break;
    }
    case State::kBodyState: {
      state_ = State::kRequestIsReady;
    }
    default: {
      break;
    }
  }
}

void RequestParser::SetNextChunkedState() {
  switch (chunked_state_) {
    case ChunkedState::kContentLengthState: {
      chunked_state_ = ChunkedState::kContentState;
      break;
    }
    case ChunkedState::kContentState: {
      chunked_state_ = ChunkedState::kContentLengthState;
      break;
    }
    default: {
      break;
    }
  }
}

bool RequestParser::IsHeaderTrigger() {
  return (state_ == State::kHeaderState &&
          save_buffer_.empty() &&
          (processing_str_[0] == '\n' ||
          processing_str_[0] == '\r'));
}

bool RequestParser::IsRequestIsReadyTrigger() {
  return (state_ == State::kBodyState &&
          save_buffer_.empty() &&
          (processing_str_[0] == '\n' ||
          processing_str_[0] == '\r'));
}

// -----------------------------------------------------------------------------

// Methods Handlers ------------------------------------------------------------

void RequestParser::HandleGet() {
  request_.SetMethod("GET");
}

void RequestParser::HandleHead() {
  request_.SetMethod("HEAD");
}

void RequestParser::HandlePost() {
  expect_body_ = true;
  request_.SetMethod("POST");
}

void RequestParser::HandlePut() {
  expect_body_ = true;
  request_.SetMethod("PUT");
}

// -----------------------------------------------------------------------------

// Headers Handlers ------------------------------------------------------------

void RequestParser::HandleAcceptCharsets(const std::string& value) {
  request_.SetHeader(ACCEPT_CHARSET, value);
}

void RequestParser::HandleAcceptLanguage(const std::string& value) {
  request_.SetHeader(ACCEPT_LANGUAGE, value);
}

void RequestParser::HandleAllow(const std::string& value) {
  request_.SetHeader(ALLOW, value);
}

void RequestParser::HandleAuthorization(const std::string& value) {
  request_.SetHeader(AUTHORIZATION, value);
}

void RequestParser::HandleContentLanguage(const std::string& value) {
  request_.SetHeader(CONTENT_LANGUAGE, value);
}

void RequestParser::HandleContentLength(const std::string& value) {
  if (!RequestParser::IsNumber(&value)) {
    throw BadRequest();
  }
  expect_body_           = true;
  content_length_header_ = true;
  content_length_        = std::stoi(value, nullptr, 10);
  request_.SetHeader(CONTENT_LENGTH, value);
}

void RequestParser::HandleContentLocation(const std::string& value) {
  request_.SetHeader(CONTENT_LOCATION, value);
}

void RequestParser::HandleContentType(const std::string& value) {
  request_.SetHeader(CONTENT_TYPE, value);
}

void RequestParser::HandleDate(const std::string& value) {
  request_.SetHeader(DATE, value);
}

void RequestParser::HandleHost(const std::string& value) {
  if (request_.GetHeaders().find(HOST) != request_.GetHeaders().end()) {
    throw BadRequest();
  }
  request_.SetHeader(HOST, value);
}

void RequestParser::HandleLastModified(const std::string& value) {
  request_.SetHeader(LAST_MODIFIED, value);
}

void RequestParser::HandleLocation(const std::string& value) {
  request_.SetHeader(LOCATION, value);
}

void RequestParser::HandleReferer(const std::string& value) {
  request_.SetHeader(REFERER, value);
}

void RequestParser::HandleRetryAfter(const std::string& value) {
  request_.SetHeader(RETRY_AFTER, value);
}

void RequestParser::HandleServer(const std::string& value) {
  request_.SetHeader(SERVER, value);
}

void RequestParser::HandleTransferEncoding(const std::string& value) {
  if (value != "chunked") {
    throw BadRequest();
  }
  chunked_ = true;
  request_.SetHeader(TRANSFER_ENCODING, value);
}

void RequestParser::HandleUserAgent(const std::string& value) {
  request_.SetHeader(USER_AGENT, value);
}

void RequestParser::HandleWWWAuthenticate(const std::string& value) {
  request_.SetHeader(WWW_AUTHENTICATE, value);
}

// -----------------------------------------------------------------------------
