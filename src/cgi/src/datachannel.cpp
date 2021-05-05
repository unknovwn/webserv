//
// Created by Gueren Drive on 4/20/21.
//

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "datachannel.hpp"

DataChannel::DataChannel(const std::string &file_name)
: file_name_(file_name) {
  std::fstream file(file_name.c_str(), std::ios_base::trunc | std::ios_base::in
                    | std::ios_base::out);
  file.close();
}

DataChannel::~DataChannel() {
  remove(file_name_.c_str());
}

// Modifiers
void DataChannel::PutData(const char *data) {
  std::ofstream out(file_name_,
                    std::ofstream::out | std::ofstream::trunc);
  if (out.is_open()) {
    out << data;
  }
  out.close();
}

std::string DataChannel::ReadData() const {
  std::ifstream in(file_name_);
  std::string   line;
  std::string   data;

  if (in.is_open()) {
    while (std::getline(in, line)) {
      data.append(line);
    }
  }
  in.close();
  return std::string(data);
}

// Element access --------------------------------------------------------------
int DataChannel::CreateFd() const {
  int fd = open(file_name_.c_str(), O_RDWR);
  if (fd < 0) {
    throw ResourcesError();
  }
  return fd;
}

// Exception -------------------------------------------------------------------
const char *DataChannel::ResourcesError::what() const noexcept {
  return "lack of resources";
}

const char *DataChannel::ChannelIsFull::what() const noexcept {
  return "channel is full";
}
