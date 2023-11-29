#include <cstdio>
// inheritance example
class RuntimeExample {
public:
  virtual void placeOrder() {
    printf("RuntimeExample::placeOrder()\n");
  }
};

class SpecificRuntimeExample : public RuntimeExample {
public:
  void placeOrder() override {
    printf("SpecificRuntimeExample::placeOrder()\n");
  }
};

//CRTP Example
template<typename actual_type>
class CRTPExample {
public:
  void placeOrder() {
    // cast the this pointer to the actual type
    static_cast<actual_type *>(this)->actualPlaceOrder();
  }

  void actualPlaceOrder() {
    printf("CRTPExample::actualPlaceOrder()\n");
  }
};

class SpecificCRTPExample : public CRTPExample<SpecificCRTPExample> {
public:
  void actualPlaceOrder() {
    printf("SpecificCRTPExample::actualPlaceOrder()\n");
  }
};

/*
Pros:
1. The CRTP enables static polymorphism, where the method to be called is determined at compile time, not at runtime.
2. NO Virtual Function Table Overhead: Since CRTP does not rely on virtual functions, it deos not incur the overhead associated with the VTable, which is 
used in dynamic polymorphism for runtime method resolution.
3. CRTP provides a safe way to downcast from the base to the derived class without the need for "dynamic_cast" or RTTI.
Cons:
1. Code complexity.
2. Maintenance Challenges
3. Compile-time errors are harder to debug than runtime errors.
4. Increased Compile Time.
5. Limited to static polymorphism.
*/
int main(int, char **) {
  RuntimeExample *runtime_example = new SpecificRuntimeExample();
  runtime_example->placeOrder();

  CRTPExample <SpecificCRTPExample> crtp_example;
  crtp_example.placeOrder();

  return 0;
}
