#pragma once

#include <string>
#include <chrono>
#include <ctime>

namespace Common {
  typedef int64_t Nanos;

  constexpr Nanos NANOS_TO_MICROS = 1000;
  constexpr Nanos MICROS_TO_MILLIS = 1000;
  constexpr Nanos MILLIS_TO_SECS = 1000;
  constexpr Nanos NANOS_TO_MILLIS = NANOS_TO_MICROS * MICROS_TO_MILLIS;
  constexpr Nanos NANOS_TO_SECS = NANOS_TO_MILLIS * MILLIS_TO_SECS;
  
  // This fucntion returns the current time since the epoch. 
  inline auto getCurrentNanos() noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
  }
  
  // This function retrurns a reference to a string object.
  inline auto& getCurrentTimeStr(std::string* time_str) {
    const auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    time_str->assign(ctime(&time)); //replace time_str with the current time in ctime format.
    if(!time_str->empty())
      time_str->at(time_str->length()-1) = '\0'; // replace the last character with the null character.
    return *time_str;
  }
}
