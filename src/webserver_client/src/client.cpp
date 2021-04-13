//
// Created by Gueren Drive on 4/10/21.
//

#include "client.hpp"

// Constructors, destructor, operator= -----------------------------------------

Client::Client()
: request_parser_(), timer_(), lifetime_(), address_() {}

Client::Client(size_t lifetime, const std::string& address)
: request_parser_(), timer_(), lifetime_(lifetime), address_(address) {}

// ---------------------------------------------------------------------------

// Getters, Setters ----------------------------------------------------------

void Client::set_lifetime(size_t lifetime) {
  lifetime_ = lifetime;
}

const std::string& Client::get_address() const {
  return address_;
}

// ---------------------------------------------------------------------------

// Member-functions ----------------------------------------------------------

bool Client::Alive() {
  return timer_.elapsed() < lifetime_;
}

void Client::ResetTimer() {
  timer_.reset();
}

// ---------------------------------------------------------------------------
