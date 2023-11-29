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

int main(int, char **) {
  RuntimeExample *runtime_example = new SpecificRuntimeExample();
  runtime_example->placeOrder();

  CRTPExample <SpecificCRTPExample> crtp_example;
  crtp_example.placeOrder();

  return 0;
}
