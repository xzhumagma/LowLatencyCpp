#include<iostream>

class Myclass{
    public:
    Myclass(int value){}
};

void functionTakingMyclass(Myclass obj)
{
    std::cout << "functionTakingMyclass called" << std::endl;
}

class Myclass2{
    public:
    explicit Myclass2(int value){}
};

void functionTakingMyclass2(Myclass2 obj)
{
    std::cout << "functionTakingMyclass2 called" << std::endl;
}

int main()
{
    functionTakingMyclass(10);
    // functionTakingMyclass2(10); // this will not compile
    functionTakingMyclass2(Myclass2(10));
    return 0;
}