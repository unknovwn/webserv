//
// Created by Gueren Drive on 4/3/21.
//

#include <fcntl.h>

#include <fstream>
#include <iostream>


#include "gtest/gtest.h"

#define BASE_GET_FILE "../base_get.txt"
#define BASE_CHUNKED_FILE "../base_chunked.txt"
#define BASE_HEAD_POST_FILE "../base_head_post.txt"

#include "request_parser.hpp"

#define BUFFER_SIZE_ONE 1
#define BUFFER_SIZE_TWO 2
#define BUFFER_SIZE_NINE 9
#define BUFFER_SIZE_BIG 2000

TEST(GetBy1, base_get_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_ONE + 1];
  int      read_counter;
  int      fd = open(BASE_GET_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_ONE)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  if (request == nullptr) {
    FAIL();
  }
  auto headers(request->GetHeaders());
  ASSERT_STREQ("GET", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("utf-8, iso-8859-1;q=0.5, *;q=0.1",
               headers["Accept-Charset"].c_str());
  ASSERT_STREQ("fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5",
               headers["Accept-Language"].c_str());
  ASSERT_STREQ("GET, HEAD, POST", headers["Allow"].c_str());
  ASSERT_STREQ("<type> <user data>", headers["Authorization"].c_str());
  ASSERT_STREQ("de-DE, en-CA", headers["Content-Language"].c_str());
  delete request;
}

TEST(GetBy2, base_get_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_TWO + 1];
  int      read_counter;
  int      fd = open(BASE_GET_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_TWO)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  if (request == nullptr) {
    FAIL();
  }
  auto headers(request->GetHeaders());
  ASSERT_STREQ("GET", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("utf-8, iso-8859-1;q=0.5, *;q=0.1",
               headers["Accept-Charset"].c_str());
  ASSERT_STREQ("fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5",
               headers["Accept-Language"].c_str());
  ASSERT_STREQ("GET, HEAD, POST", headers["Allow"].c_str());
  ASSERT_STREQ("<type> <user data>", headers["Authorization"].c_str());
  ASSERT_STREQ("de-DE, en-CA", headers["Content-Language"].c_str());
  delete request;
}

TEST(GetBy9, base_get_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_NINE + 1];
  int      read_counter;
  int      fd = open(BASE_GET_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_NINE)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  if (request == nullptr) {
    FAIL();
  }
  auto headers(request->GetHeaders());
  ASSERT_STREQ("GET", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("utf-8, iso-8859-1;q=0.5, *;q=0.1",
               headers["Accept-Charset"].c_str());
  ASSERT_STREQ("fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5",
               headers["Accept-Language"].c_str());
  ASSERT_STREQ("GET, HEAD, POST", headers["Allow"].c_str());
  ASSERT_STREQ("<type> <user data>", headers["Authorization"].c_str());
  ASSERT_STREQ("de-DE, en-CA", headers["Content-Language"].c_str());
  delete request;
}

TEST(GetByBIG, base_get_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_BIG + 1];
  int      read_counter;
  int      fd = open(BASE_GET_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_BIG)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  if (request == nullptr) {
    FAIL();
  }
  auto headers(request->GetHeaders());
  ASSERT_STREQ("GET", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("utf-8, iso-8859-1;q=0.5, *;q=0.1",
               headers["Accept-Charset"].c_str());
  ASSERT_STREQ("fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5",
               headers["Accept-Language"].c_str());
  ASSERT_STREQ("GET, HEAD, POST", headers["Allow"].c_str());
  ASSERT_STREQ("<type> <user data>", headers["Authorization"].c_str());
  ASSERT_STREQ("de-DE, en-CA", headers["Content-Language"].c_str());
  delete request;
}

TEST(PutChunkedBy1, base_put_chunked_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_ONE + 1];
  int      read_counter;
  int      fd = open(BASE_CHUNKED_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_ONE)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  auto headers(request->GetHeaders());
  if (request == nullptr) {
    FAIL();
  }
  ASSERT_STREQ("PUT", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("text/plain", headers["Content-Type"].c_str());
  ASSERT_STREQ("chunked", headers["Transfer-Encoding"].c_str());
  ASSERT_STREQ("Mozilla\nDeveloper\nNetwork\n", request->GetBody().c_str());
  delete request;
}

TEST(PutChunkedBy2, base_put_chunked_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_TWO + 1];
  int      read_counter;
  int      fd = open(BASE_CHUNKED_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_TWO)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  auto headers(request->GetHeaders());
  if (request == nullptr) {
    FAIL();
  }
  ASSERT_STREQ("PUT", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("text/plain", headers["Content-Type"].c_str());
  ASSERT_STREQ("chunked", headers["Transfer-Encoding"].c_str());
  ASSERT_STREQ("Mozilla\nDeveloper\nNetwork\n", request->GetBody().c_str());
  delete request;
}

TEST(PutChunkedBy9, base_put_chunked_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_NINE + 1];
  int      read_counter;
  int      fd = open(BASE_CHUNKED_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_NINE)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  auto headers(request->GetHeaders());
  if (request == nullptr) {
    FAIL();
  }
  ASSERT_STREQ("PUT", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("text/plain", headers["Content-Type"].c_str());
  ASSERT_STREQ("chunked", headers["Transfer-Encoding"].c_str());
  ASSERT_STREQ("Mozilla\nDeveloper\nNetwork\n", request->GetBody().c_str());
  delete request;
}

TEST(PutChunkedByBig, base_put_chunked_method) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_BIG + 1];
  int      read_counter;
  int      fd = open(BASE_CHUNKED_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_BIG)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
  }
  auto headers(request->GetHeaders());
  if (request == nullptr) {
    FAIL();
  }
  ASSERT_STREQ("PUT", request->GetMethod().c_str());
  ASSERT_STREQ("/", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("text/plain", headers["Content-Type"].c_str());
  ASSERT_STREQ("chunked", headers["Transfer-Encoding"].c_str());
  ASSERT_STREQ("Mozilla\nDeveloper\nNetwork\n", request->GetBody().c_str());
  delete request;
}

TEST(HeadPostByBig, base_head_post_methods) {
  RequestParser parser;
  Request* request;
  char     buff[BUFFER_SIZE_BIG + 1];
  int      read_counter;
  int      fd = open(BASE_HEAD_POST_FILE,  O_RDONLY);

  if (fd < 0) {
    std::cerr << "Error: can't create file" << std::endl;
  }
  while ((read_counter = read(fd, buff, BUFFER_SIZE_BIG)) > 0) {
    buff[read_counter] = '\0';
    request = parser.ParseRequest(buff);
    if (request) {
      break;
    }
  }
  auto headers(request->GetHeaders());
  if (request == nullptr) {
    FAIL();
  }
  ASSERT_STREQ("HEAD", request->GetMethod().c_str());
  ASSERT_STREQ("/.dir/porno", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("www.mgeneviv.com", headers["Host"].c_str());
  ASSERT_STREQ("10", headers["Content-Length"].c_str());
  ASSERT_STREQ("1234567891", request->GetBody().c_str());
  ASSERT_FALSE(parser.Empty());
  delete request;
  while (!parser.Empty()) {
    request = parser.ParseRequest();
  }
  ASSERT_TRUE(parser.Empty());
  headers = request->GetHeaders();
  ASSERT_STREQ("POST", request->GetMethod().c_str());
  ASSERT_STREQ("/school21/main_table", request->GetPath().c_str());
  ASSERT_STREQ("HTTP/1.1", request->GetAProtocol().c_str());
  ASSERT_STREQ("www.school21.com", headers["Host"].c_str());
  ASSERT_STREQ("27", headers["Content-Length"].c_str());
  ASSERT_STREQ("take Gleb to work in Bocal\n", request->GetBody().c_str());
  delete request;
}
