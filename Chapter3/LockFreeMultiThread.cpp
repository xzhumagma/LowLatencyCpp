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
            // currentBalance: This is a reference to a 'double' variable that holds the current value of 'balance' before any update
            // It's used both as the expected value for comparison and to store the updated value if the comparison succeeds.
            // currentBalance + amount: This is the new value of the balance if the comparison succeeds. 
            // std::memory_order_release: This is the memory order for the store operation if the comparison succeeds.
            while (!balance.compare_exchange_weak(currentBalance, currentBalance + amount, std::memory_order_release, std::memory_order_relaxed));
            // Retry if the balance has not changed.
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