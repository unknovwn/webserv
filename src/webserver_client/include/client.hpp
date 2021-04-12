//
// Created by Gueren Drive on 4/10/21.
//

#include "request_parser.hpp"
#include "timer.hpp"

#pragma once

class Client {
 public:
  [[maybe_unused]] RequestParser request_parser_;
  // Constructors, destructor, operator= ---------------------------------------
  Client();
  explicit Client(size_t lifetime);
  Client(const Client& x) = default;
  ~Client() = default;
  Client& operator=(const Client& x) = default;
  // ---------------------------------------------------------------------------

  // Getters, Setters ----------------------------------------------------------
  void set_lifetime(size_t lifetime);
  // ---------------------------------------------------------------------------

  // Member-functions ----------------------------------------------------------
  bool Alive();
  void ResetTimer();
  // ---------------------------------------------------------------------------

 private:
  Timer timer_;
  size_t lifetime_;
};
