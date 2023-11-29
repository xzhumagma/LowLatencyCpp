#include<iostream>
#include<stdexcept>
#include<cassert>
#include<cmath>

class Test{
    public:
        void mayThrow(){
            throw std::runtime_error("Exception thrown from mayThrow()");
        }

        void nonThrow() noexcept{
            std::cout << "Exception thrown from noThrow()" << std::endl;
        }
};

class Triangle{
    public:
        static double computeArea(double a, double b, double c){
            // check or triangle inequality theorem using assert
            assert(a + b > c && b + c > a && a + c > b);
            // compute area using Heron's formula
            double s = (a + b + c) / 2.0;
            return std::sqrt(s * (s - a) * (s - b) * (s - c));
        }
};
// the std::runtime_error is thrown using the throw keyword. The message "Exception thrown from mayThrow()" is passsed to the exception's constructor.
int main(){
    /*
    Test obj;

    try{
        obj.mayThrow();
    } catch (const std::exception& e){
        std::cout << "Exception caught: " << e.what() << std::endl;
    }

    try{
        obj.nonThrow();
    } catch (const std::exception& e){
        std::cout << "This will not be printed" << e.what() << std::endl;
    }
    */

    double area  = Triangle::computeArea(3,4,6);
    std::cout << "Area of triangle: " << area << std::endl;
    return 0;
}