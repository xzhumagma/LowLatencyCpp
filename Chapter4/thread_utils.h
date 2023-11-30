#pragma once

#include <iostream>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <memory>
#include <sys/syscall.h>
/*
The affinity mask of a CPU is a data structure, typically a bit mask, used to specify or represent the affinity
of a process or thread to one or more CPUs within a system. CPU affinity, also known as processor affinity, defines how and which
specific CPUs a process or thread is allowed to use.
*/
namespace Common
{
  /// Set affinity for current thread to be pinned to the provided core_id.
  inline auto setThreadCore(int core_id) noexcept
  {
    cpu_set_t cpuset; // a set of cpu

    CPU_ZERO(&cpuset);         //  initializes the CPU set set to be the empty set.
    CPU_SET(core_id, &cpuset); // adds the cpu with the ID 'core_id' to the set 'cpuset'.

    return (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0);
    // to set the CPU affinity mask of the current thread to the specified cpuset.
  }

  /// Creates a thread instance, sets affinity on it, assigns it a name and
  /// passes the function to be run on that thread as well as the arguments to the function.
  template <typename T, typename... A>
  // type name T, the type of function that the thread will exceute.
  // type name A, variadic template parameter pack, the types of the arguments to the function.
  inline auto createAndStartThread(int core_id, const std::string &name, T &&func, A &&...args)
  {
    auto t = [core_id, name, &func, &args...]()
    {
      try
      {
        if (core_id >= 0 && !setThreadCore(core_id))
        {
          std::cerr << "Failed to set core affinity for " << name << std::endl;
          throw ::std::runtime_error("Failed to set core affinity for " + name); // if this block is uncatched by the block, then it will trigger
                                                                                 // the std::terminate(), and then calls the std::abort() to terminate the program.
        }
        std::cerr << "Set core affinity for " << name << " " << pthread_self() << " to " << core_id << std::endl;
        // the std::cerr is unbuffered and give the feedback immediately.
        std::forward<T>(func)((std::forward<A>(args))...); // excutes thr function T with the arguments A.
      }
      catch (const std::exception &e)
      {
        std::cerr << "Exception in thread " << name << ": " << e.what() << std::endl;
      }
    };

    std::this_thread::sleep_for(std::chrono::seconds(1)); // set the thread to sleep for 1 second.

    return std::make_unique<std::thread>(std::move(t)); // returns a unique pointer to it.
  }
}
