#include "thread_utils.h"

auto dummyFunction(int a, int b, bool sleep)
{
  std::cout << "dummyFunction(" << a << "," << b << ")" << std::endl;
  std::cout << "dummyFunction output=" << a + b << std::endl;

  if (sleep)
  {
    std::cout << "dummyFunction sleeping..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }

  std::cout << "dummyFunction done." << std::endl;
}

int main(int, char **)
{
  // Get the number of available cores
  unsigned int numCores = std::thread::hardware_concurrency();
  std::cout << "This machine supports concurrency with " << numCores << " cores available" << std::endl;
  try{
  auto t1 = Common::createAndStartThread(129, "dummyFunction1", dummyFunction, 12, 21, false);
  auto t2 = Common::createAndStartThread(1, "dummyFunction2", dummyFunction, 15, 51, true);
  t1->join();
  t2->join();
  }
  catch(const std::exception& e){
    std::cerr << "Error: " << e.what() << std::endl;
  }
  
  std::cout << "main exiting." << std::endl;

  return 0;
}
