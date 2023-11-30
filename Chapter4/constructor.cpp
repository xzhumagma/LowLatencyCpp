/*
This document give examples of constructors, copy and move constructors, copy and move assignment operators, and destructors.
*/
#include<iostream>
class SimpleArray{
    private:
      int *data;
      size_t size;
    public:
    // Constructor
    SimpleArray(size_t sz) : size(sz), data(new int[sz]) // the member initializer list.
    {
        for (auto i = 0;i<sz;i++)
        {
            data[i] = 0;
        }
    }
    
    // Destructor
    ~SimpleArray()
    {
        delete[] data;
    }

    void display()
    {
        std::cout << "SimpleArray: ";
        for (auto i = 0;i<size;i++)
        {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }

    // Copy Constructor
    SimpleArray(const SimpleArray &other): size(other.size), data(new int[other.size])
    {
        for (auto i = 0;i<other.size;i++)
        {
            data[i] = other.data[i];
        }
    }

    // Move Constructor
    SimpleArray(SimpleArray &&other): size(other.size), data(other.data)
    {
        other.data = nullptr;
        other.size = 0;
    }

    // Copy Assignment Operator
    SimpleArray & operator = (const SimpleArray &other)
    {
        if (this != &other)
        {
            delete[] data;
            this->size = other.size;
            this->data = new int[other.size];
            std::copy(other.data,other.data+size,data);
        }
        return *this;
    }
    
    // Move Assignment Operator
    SimpleArray & operator = (SimpleArray &&other)
    {
        if (this != &other)
        {
            delete[] data;
            this->size = other.size;
            this->data = other.data;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};
// if it is a lvalue (an object with a name, like a variable), the compiler selects the copy assignment operator
// if it is a rvalue (an object without a name, like a temporary object), the compiler selects the move assignmen operator

// lvalue(locator value)
/*
Identifiable: take the address by &.
Assignable: can appear on the left-hand side of an assignment statement. int x = 5;
Example: variables, references, dereference pointers, array elements
*/
// rvalue (Right value)
/*
temporary value that does not persist beyond the expression that uses it.
Temporary: cannot take the address of a temporary object.
Not Assignable: 
Literal constants return values of fucntions, expression like 'x+y'.
*/
int main()
{
    // using the Copy constructor
    SimpleArray array1(10);
    array1.display();
    // copy construtor
    SimpleArray array2(array1);
    array2.display();
    array1.display();
    // move constructor
    // std::move is a function that takes a reference to an object and returns an rvalue reference to that object.
    SimpleArray array3(std::move(array1));
    array3.display();
    array1.display();// the array1 is empty now.
    // copy assignment operator
    SimpleArray array4 = array2;
    array4.display();
    array2.display();
    //move assignment operator
    array4 = SimpleArray(20);
    array4.display();
    array4 = std::move(array2);
    array4.display();
    array2.display();
    return 0;
}