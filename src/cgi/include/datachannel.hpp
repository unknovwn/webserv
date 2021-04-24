//
// Created by Gueren Drive on 4/20/21.
//

#pragma once

#ifndef CGIREQUESTRESPONSE_DATACHANNEL_HPP
#define CGIREQUESTRESPONSE_DATACHANNEL_HPP

#include <string>
#include <fstream>

class DataChannel {
 public:
  DataChannel() = delete;
  DataChannel(const std::string &file_name);
  DataChannel(const DataChannel &x) = delete;
  virtual ~DataChannel();

  DataChannel &operator=(const DataChannel &x) = delete;

  // Modifiers
  void PutData(const char *data);
  std::string ReadData() const;

  // Element access
  int CreateFd() const;

  // Exception
  class ResourcesError {
    const char* what() const noexcept;
  };

  class ChannelIsFull {
    const char* what() const noexcept;
  };

 private:
  const std::string file_name_;
};

#endif //CGIREQUESTRESPONSE_DATACHANNEL_HPP
