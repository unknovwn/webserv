//
// Created by Gueren Drive on 4/10/21.
//

#include "client.hpp"

// Constructors, destructor, operator= -----------------------------------------

Client::Client()
: request_parser_(), timer_(), lifetime_() {}

Client::Client(size_t lifetime)
: request_parser_(), timer_(), lifetime_(lifetime) {}

// ---------------------------------------------------------------------------

// Getters, Setters ----------------------------------------------------------

void Client::set_lifetime(size_t lifetime) {
  lifetime_ = lifetime;
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
