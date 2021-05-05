//
// Created by Gueren Drive on 4/20/21.
//

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <utility>

#include "cgi.hpp"
#include "datachannel.hpp"

Cgi::Cgi(std::string cgi_bin_path) : cgi_bin_path_(std::move(cgi_bin_path)) {}

// Operations
Response Cgi::CreateResponse(Request &request) {
  // Create args and envs
  char **args = this->CreateArgs();
  char **env = this->CreateEnv(request);

  // Put body to file
  DataChannel request_data(".request_data_cache");
  request_data.PutData(request.GetBody().c_str());

  // file for cgi output
  DataChannel response_data(".response_data_cache");

  pid_t pid;
  int status = 0;
  int request_data_fd  = request_data.CreateFd();
  int response_data_fd = response_data.CreateFd();

  // execute cgi script
  pid = fork();
  if (pid) {
    close(request_data_fd);
    close(response_data_fd);
  }
  if (!pid) {
    dup2(request_data_fd, 0);
    dup2(response_data_fd, 1);
    status = execve(cgi_bin_path_.c_str(), args, env);
    close(request_data_fd);
    close(response_data_fd);
    exit(status);
  }
  waitpid(pid, &status, 0);
  std::string response_str(response_data.ReadData());

  Cgi::Free2DMatrix<char>(args);
  Cgi::Free2DMatrix<char>(env);

  // TODO(gdrive): Need parse response_str --> class Response
  Response response(Response::StatusCodes::kOk);
  response.AddToBody(response_str);
  return response;
}

char **Cgi::CreateArgs() const {
  char **args = static_cast<char**>(malloc(sizeof(char*) * (1 + 1)));

  args[0] = strdup(cgi_bin_path_.c_str());
  args[1] = nullptr;
  return args;
}

char **Cgi::CreateEnv(Request &request) const {
  std::map<std::string, std::string> env;
  std::map<std::string, std::string> headers(request.GetHeaders());
  char pwd[1024];
  getcwd(pwd, 1024);
  size_t found_index;

  env["AUTH_TYPE"] = headers.count("Authorization")
                     ? headers["Authorization"] : "";

  // 16key may not be decoded
  env["REMOTE_USER"]  = env["AUTH_TYPE"] == "Basic" ? "16key" : "";
  env["REMOTE_IDENT"] = env["AUTH_TYPE"] == "Basic" ? "16key" : "";

  env["CONTENT_LENGTH"] = headers.count("Content-Length")
                         ? headers["Content-Length"] : "";

  env["CONTENT_TYPE"] = headers.count("Content-Type")
                        ? headers["Content-Type"] : "";

  env["GATEWAY_INTERFACE"] = "CGI/1.1";
  env["SERVER_PROTOCOL"]   = "HTTP/1.1";
  env["SERVER_SOFTWARE"]   = "webserv/1.0";

  env["REQUEST_URI"]  = request.GetPath();
  env["QUERY_STRING"] = "";

  // [uri?arguments] --> REQUEST_URI = [uri], QUERY_STRING = [?arguments]
  // [?arguments] may not be decoded
  found_index = env["REQUEST_URI"].find_first_of('?');
  if (found_index != std::string::npos) {
    env["QUERY_STRING"] = env["REQUEST_URI"].substr(found_index + 1);
    env["REQUEST_URI"].resize(found_index);
  }

  env["REQUEST_METHOD"] = request.GetMethod();
  env["SCRIPT_NAME"]    = request.GetPath();

  // [/dir/bin] --> SCRIPT_NAME = [bin]
  found_index = env["SCRIPT_NAME"].find_last_of('/');
  if (found_index != std::string::npos) {
    env["SCRIPT_NAME"] = env["SCRIPT_NAME"].substr(found_index + 1);
  }

  env["PATH_INFO"]         = request.GetPath();
  env["PATH_TRANSLATED"]   = pwd + env["PATH_INFO"];

  // TODO(gdrive): Need to add
//  env["SERVER_NAME"] = host.getName();
//  env["SERVER_PORT"] = std::to_string(host.getPort());
//  env["REMOTE_ADDR"] = iptoa(this->_request.getSockAddr().sin_addr.s_addr);

  // [Header-Header: value] --> env[HTTP_HEADER_HEADER] = value
  for (auto &it : headers) {
    std::string header(it.first);
    found_index = header.find('-');
    if (found_index != std::string::npos) {
      header.replace(found_index, 1, "_");
    }
    std::transform(header.begin(), header.end(), header.begin(), toupper);
    env["HTTP_" + header] = it.second;
  }

  // env[HTTP_HEADER_HEADER] = value --> [HTTP_HEADER_HEADER]=value
  char **env_result = static_cast<char**>(malloc(sizeof(char*)
                                                 * (env.size() + 1)));
  if (!env_result) {
    throw Cgi::ResourceError();
  }
  size_t i = 0;
  for (auto &it : headers) {
    env_result[i] = strdup(std::string(it.first + "=" + it.second).c_str());
    ++i;
  }
  env_result[i] = nullptr;
  return env_result;
}

// Exception
const char *Cgi::ResourceError::what() const noexcept {
  return "can't allocate memory";
}
