#pragma once

#include <iostream>
#include <vector>
#include <atomic>

#include "macros.h"
/*
Purpose of std::atomic:
1. safe concurrent access. the object are executed as a single, indivisible operation.
2. It guarantees that a modification made by one thread is visible to other threads in a predicatable manner, ensuring 
proper synchronization of shared data.
*/
namespace Common {
  template<typename T>
  class LFQueue final {
  public:
    LFQueue(std::size_t num_elems) :
        store_(num_elems, T()) /* pre-allocation of vector storage. */ {
    }
    // returns a pointer to the position in the queue where the next element can be written.
    auto getNextToWriteTo() noexcept {
      return &store_[next_write_index_];
    }
    // advances the write index.
    auto updateWriteIndex() noexcept {
      next_write_index_ = (next_write_index_ + 1) % store_.size();
      num_elements_++;
    }
    // returns a pointer to the position in the queue where the next element could be read. It returns 'nullptr' if the 
    // queue is empty.
    auto getNextToRead() const noexcept -> const T * {
      return (size() ? &store_[next_read_index_] : nullptr);
    }
    // advances the read index
    auto updateReadIndex() noexcept {
      next_read_index_ = (next_read_index_ + 1) % store_.size();
      ASSERT(num_elements_ != 0, "Read an invalid element in:" + std::to_string(pthread_self()));
      num_elements_--;
    }
    // returns the current numbrer of elements in the queue.
    auto size() const noexcept {
      return num_elements_.load();
    }

    // Deleted default, copy & move constructors and assignment-operators.
    LFQueue() = delete; //constructor

    LFQueue(const LFQueue &) = delete; // copy constructor

    LFQueue(LFQueue &&) = delete; // move constructor

    LFQueue &operator=(const LFQueue &) = delete; // copy assignment operator

    LFQueue &operator=(LFQueue &&) = delete; // move assignment operator

  private:
    std::vector<T> store_;

    std::atomic<size_t> next_write_index_ = {0};
    std::atomic<size_t> next_read_index_ = {0};

    std::atomic<size_t> num_elements_ = {0};
  };
}
