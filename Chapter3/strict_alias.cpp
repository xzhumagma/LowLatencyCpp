#include <cstdio>
#include <cstdint>
#include <iostream>

int main() {
  double x = 100.2;
  const auto orig_x = x; // declares a variable orig_x and initializes it with the current value of x. constant means that orig_x cannot be changed later on.

  auto x_as_ui = (uint64_t *) (&x); // (&x pointer to 'double') cast to (uint64_t *) pointer to 'unsigned long long int', *x_as_ui will be uint64_t
  // it will be a number that represents how "x" is stored in memory according to the IEEE 754 standard.
  *x_as_ui |= 0x8000000000000000; // hexadecimal number, only the most significant bit (MSB) is set to 1, and all other bits are set to 0.
 // in IEEE 754 format for 'double', the MSB is the sign bit, with 1 indicating a negative number and 0 indicating a positive number. 
  printf("orig_x:%0.2f x:%0.2f &x:%p &x_as_ui:%p\n", orig_x, x, &x, x_as_ui);
  
  double y = 100.4;

  std::cout << "Original value of y: " << y << std::endl;

  // Reinterpret the memory location of y as an unsigned long long int
  auto y_as_ui = reinterpret_cast<uint64_t *>(&y);
  // Perform the bitwise OR operation to set the MSB to 1
  *y_as_ui |= 0x8000000000000000;

  // Reinterpret the memory location of y as a double and print the result
  std::cout << "Modified value of y: " << y  << std::endl;
  return 0;
}
