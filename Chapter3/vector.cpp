#include <cstddef>
#include<iostream>
#include<chrono>

int main() {
  const size_t size = 1024;
 // [[maybe_unused]]
  float x[size], a[size], b[size],y[size],z[size];
  // Start timing
  auto start = std::chrono::high_resolution_clock::now();
  // no vectorization
  for (size_t i = 0; i < size; ++i) {
    x[i] = a[i] + b[i];
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  std::cout << "No vectorization: " << duration.count() << " ns\n";

  auto start2 = std::chrono::high_resolution_clock::now();
  // vectorization (at most 4 elements at at time)
  for (size_t i = 0; i < size; i += 4) {
    y[i] = a[i] + b[i];
    y[i + 1] = a[i + 1] + b[i + 1];
    y[i + 2] = a[i + 2] + b[i + 2];
    y[i + 3] = a[i + 3] + b[i + 3];
  }
  auto end2 = std::chrono::high_resolution_clock::now();
  auto duration2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - start2);
  std::cout << "Vectorization: " << duration2.count() << " ns\n";

  auto start3 = std::chrono::high_resolution_clock::now();

  for (size_t i = 0;i < size;i += 8) {
    z[i] = a[i] + b[i];
    z[i + 1] = a[i + 1] + b[i + 1];
    z[i + 2] = a[i + 2] + b[i + 2];
    z[i + 3] = a[i + 3] + b[i + 3];
    z[i + 4] = a[i + 4] + b[i + 4];
    z[i + 5] = a[i + 5] + b[i + 5];
    z[i + 6] = a[i + 6] + b[i + 6];
    z[i + 7] = a[i + 7] + b[i + 7];
  }
  auto end3 = std::chrono::high_resolution_clock::now();
  auto duration3 = std::chrono::duration_cast<std::chrono::nanoseconds>(end3 - start3);
  std::cout << "Vectorization2: " << duration3.count() << " ns\n";
  return 0;
}