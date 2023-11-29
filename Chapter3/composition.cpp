#include <cstdio>
#include <vector>
#include<iostream>
#include<cmath>
#include<stdexcept>

struct Order {
  int id;
  double price;
};
// InheritanceOrderBook acquires all the members (methods and properties) of the base class std::vector<Order>
// This is not recommended because the base class std::vector<Order> is not designed to be a base class (it does not have a virtual destructor)
class InheritanceOrderBook : public std::vector<Order> {
};

// Example to show the inappropriateness of using inheritance for std::vector<Order>
void deleteOrderBook(std::vector<Order> *ob)
{
  delete ob; // deleting using a base class pointer.
}

// The compositionOrderBook has a std::vector<Order> as a member
class CompositionOrderBook {
  std::vector<Order> orders_;

public:
// does not throw exceptions
  auto size() const noexcept {
    return orders_.size();
  }
};




class Base{
  public:
      virtual void show(){
          std::cout << "Base clas show function called." << std::endl;
      }
};

class Derived : public Base{
    public:
        void show() override{
            std::cout << "Derived class show function called." << std::endl;
        }
};

class Shape{
  public:
      virtual double area() const = 0; // Pure virtual function
      virtual ~Shape() {} // Virtual destructor
};

class Circle : public Shape{
  private:
      double radius;
  public:
      Circle(double r) : radius(r) {}
      double area() const override{
        return M_PI * radius * radius;
      }
};

class Rectangle : public Shape{
  private:
      double width;
      double height;
  public:
      Rectangle(double w, double h) : width(w), height(h) {}
      double area() const override{
        return width * height;
      }
};

void printArea(const Shape& shape){
  std::cout << "Area: " << shape.area() << std::endl;
}


int main() {
  
  InheritanceOrderBook i_book;
  CompositionOrderBook c_book;

  printf("InheritanceOrderBook::size():%lu CompositionOrderBook:%lu\n", i_book.size(), c_book.size());
  
  /*
  std::vector<Order> *ob = new InheritanceOrderBook();
  deleteOrderBook(ob); // Potential undefined behavior
  return 0;
  */

 // *********************************************************************************************************************
 // The example shows the runtime polymorphsim
 /* Base *b;
 Derived d;
 b = &d;
//Runtime Polymorphism
 b->show();
 */
// *********************************************************************************************************************
// The example shows the polymorphism of computing area for different shapes
/*
Circle circle(1.0);
Rectangle rectangle(2.0, 3.0);
printArea(circle);
printArea(rectangle);
*/
return 0;

}