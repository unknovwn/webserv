//
// Created by Gueren Drive on 4/11/21.
//

#include <chrono>

#pragma once

class Timer {
 public:
  // Constructors, destructor, operator= ---------------------------------------
  Timer();
  Timer(const Timer& x) = default;
  ~Timer() = default;
  Timer& operator=(const Timer& x) = default;
  // ---------------------------------------------------------------------------

  // Member-functions ----------------------------------------------------------
  void   reset();
  size_t elapsed() const;
  // ---------------------------------------------------------------------------

 private:
  using clock_t  = std::chrono::high_resolution_clock;
  using second_t = std::chrono::duration<size_t, std::ratio<1> >;
  std::chrono::time_point<clock_t> m_beg_;
};
