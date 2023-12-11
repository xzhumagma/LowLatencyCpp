#include<iostream>
#include<thread>
#include<atomic>

class BankAccount{
     private:
         std::atomic<double> balance;
     public:
         BankAccount(): balance(0.0){}
         void deposit(double amount){
            double currentBalance = balance.load(std::memory_order_relaxed);
            while (!balance.compare_exchange_weak(currentBalance, currentBalance + amount, std::memory_order_release, std::memory_order_relaxed));
            // Retry if the balance has changed.
         }

         void withdraw(double amount){
             double currentBalance = balance.load(std::memory_order_relaxed);
             while (!balance.compare_exchange_weak(currentBalance, currentBalance - amount, std::memory_order_release, std::memory_order_relaxed));
             // Retry if the balance has changed.
         }

         double getBalance(){
             return balance.load(std::memory_order_relaxed);
         }
};

int main(){
    auto start  = std::chrono::high_resolution_clock::now();
    BankAccount account;
    std::thread depositThread1([&]{
        for (int i = 0; i < 1000000; i++)
        {
            account.deposit(2.0);
        }
    });
    std::thread withdrawThread1([&]{
        for (int i = 0; i < 500000; i++)
        {
            account.withdraw(1.0);
        }
    });
    std::thread withdrawThread2([&]{
        for (int i = 0; i < 500000; i++)
        {
            account.withdraw(1.0);
        }
    });
    
    depositThread1.join();
    withdrawThread1.join();
    withdrawThread2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " microseconds" << std::endl;
    std::cout << "Final balance: " << account.getBalance() << std::endl;
    return 0;
}