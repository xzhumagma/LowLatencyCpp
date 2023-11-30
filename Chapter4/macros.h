#pragma once

#include <cstring>
#include <iostream>

#define LIKELY(x) __builtin_expect(!!(x), 1) // tells the compiler that the expression 'x' is expected to be true most of the time.
#define UNLIKELY(x) __builtin_expect(!!(x), 0) // tells the compiler that the expression 'x' is expected to be false most of the time.
// !! ensures that the expression is converted to a boolean value

// the compiler replaces the function call with the actual code of the function.
inline auto ASSERT(bool cond, const std::string &msg) noexcept {
  if (UNLIKELY(!cond)) {
    std::cerr << "ASSERT : " << msg << std::endl; // ouitput the error message to the standard error stream.

    exit(EXIT_FAILURE); // terminates the program with a failure status.
  }
}
// The fucntion is used for situations where an unrecoverable error has occurred. It prints a provided error message and then 
// terminates the program.
inline auto FATAL(const std::string &msg) noexcept { 
  std::cerr << "FATAL : " << msg << std::endl;

  exit(EXIT_FAILURE);
}
