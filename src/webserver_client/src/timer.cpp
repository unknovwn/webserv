//
// Created by Gueren Drive on 4/11/21.
//

#include "timer.hpp"

// Constructors, destructor, operator= -----------------------------------------

Timer::Timer() : m_beg_(clock_t::now()) {}

// -----------------------------------------------------------------------------

// Member-functions ----------------------------------------------------------

void   Timer::reset() {
  m_beg_ = clock_t::now();
}

size_t Timer::elapsed() const {
  return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg_).count();
}

// -----------------------------------------------------------------------------
