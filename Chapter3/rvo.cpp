#include <iostream>

struct LargeClass {
  int i;
  char c;
  double d;
  void display() const{
    std::cout << "i: " << i << ", c: " << c << ", d: " << d << std::endl;
  }
};

auto rvoExample(int i, char c, double d) {
  return LargeClass{i, c, d};
}

struct LargeClassWithConstructor{
  int i;
  char c;
  double d;

  // copy constructor
  LargeClassWithConstructor(const LargeClassWithConstructor& other) : i(other.i), c(other.c), d(other.d) {
    std::cout << "Copy constructor called" << std::endl;
  }
  LargeClassWithConstructor(int i, char c, double d) : i(i), c(c), d(d) {}

  void display() const{
    std::cout << "i: " << i << ", c: " << c << ", d: " << d << std::endl;
  }
};

LargeClassWithConstructor createLargeClass(int i , char c, double d){
  LargeClassWithConstructor temp(i,c,d);
  return temp;
}



int main() {
  LargeClass lc_obj = rvoExample(10, 'c', 3.14);
  lc_obj.display();
  LargeClassWithConstructor lcc_obj = createLargeClass(10, 'c', 3.14);
  LargeClassWithConstructor lcc_obj2 = LargeClassWithConstructor(lcc_obj);
  lcc_obj.display();
  lcc_obj2.display();
  lcc_obj.i = 20;
  lcc_obj.display();
  lcc_obj2.display();
}