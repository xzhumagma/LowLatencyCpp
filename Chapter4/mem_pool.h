#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "macros.h"

namespace Common {
  template<typename T>
  class MemPool final { // the final means that this class cannot be inherited.
  public:
    explicit MemPool(std::size_t num_elems) : // the explicit means that the constructor cannot be used for implicit conversions and copy-initialization.
    // it's often a good practice to declare single-argument constructors as explicit to avoid unintentional implicit conversions.
        store_(num_elems, {T(), true}) /* pre-allocation of vector storage. */ 
        //  initialize the vector with num_elems elements, each of which is initialized with the value {T(), true}.
        {
      ASSERT(reinterpret_cast<const ObjectBlock *>(&(store_[0].object_)) == &(store_[0]), "T object should be first member of ObjectBlock.");
    }

    template<typename... Args>
    T *allocate(Args... args) noexcept {
      auto obj_block = &(store_[next_free_index_]);
      // stores_ {T(),True}
      ASSERT(obj_block->is_free_, "Expected free ObjectBlock at index:" + std::to_string(next_free_index_));
      // if the obj_block is free, then we continue the following code
      T *ret = &(obj_block->object_); // the address of the object_ is assinged to the pointer ret.
      ret = new(ret) T(args...); // placement new.
      obj_block->is_free_ = false; // the obj_block is not free any more.

      updateNextFreeIndex(); // update the next_free_index_.

      return ret;
    }

    auto deallocate(const T *elem) noexcept {
      // elem_index is the element index of the elem in the store_.
      const auto elem_index = (reinterpret_cast<const ObjectBlock *>(elem) - &store_[0]);
      ASSERT(elem_index >= 0 && static_cast<size_t>(elem_index) < store_.size(), "Element being deallocated does not belong to this Memory pool.");
      ASSERT(!store_[elem_index].is_free_, "Expected in-use ObjectBlock at index:" + std::to_string(elem_index));
      store_[elem_index].is_free_ = true;
    }

    bool isFreeAtIndex(std::size_t index) const {
        if (index >= store_.size()) {
            throw std::out_of_range("Index out of range");
        }
        return store_[index].is_free_;
    }

    // Deleted default, copy & move constructors and assignment-operators.
    MemPool() = delete; /*the line deletes the default constructor of the MemPool class. It means that you cannot create an object of the 
    MemPool without arguments */

    MemPool(const MemPool &) = delete; // deletes the copy constructor.

    MemPool(MemPool &&) = delete;// deletes the move constructor.

    MemPool &operator=(const MemPool &) = delete; // deletes the copy assignment operator.

    MemPool &operator=(MemPool &&) = delete; // deletes the move assignment operator.

  private:
    auto updateNextFreeIndex() noexcept {
      const auto initial_free_index = next_free_index_;
      while (!store_[next_free_index_].is_free_) {
        ++next_free_index_;
        if (UNLIKELY(next_free_index_ == store_.size())) { // hardware branch predictor should almost always predict this to be false any ways.
          next_free_index_ = 0; // circular search over the store_
        }
        if (UNLIKELY(initial_free_index == next_free_index_)) {
          ASSERT(initial_free_index != next_free_index_, "Memory Pool out of space."); // Full Pool Check.
        }
      }
    }

    // It is better to have one vector of structs with two objects than two vectors of one object.
    // Consider how these are accessed and cache performance.
    struct ObjectBlock {
      T object_;
      bool is_free_ = true;
    };

    // We could've chosen to use a std::array that would allocate the memory on the stack instead of the heap.
    // We would have to measure to see which one yields better performance.
    // It is good to have objects on the stack but performance starts getting worse as the size of the pool increases.
    std::vector<ObjectBlock> store_;

    size_t next_free_index_ = 0;
  };
}
