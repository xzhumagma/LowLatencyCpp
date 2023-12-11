// This cpp is for the different multithread secnario. 
#include<iostream>
#include<mutex>
#include<semaphore>
#include<thread>

class BankAccount
{
    private:
      double balance;
      std::mutex mtx;
    public:
       BankAccount(): balance(0.0){}

       void deposit(double amount)
       {
         // Lock the mutex to ensure exclusive access to the balance
         std::lock_guard<std::mutex> lock(mtx);
         // Simulate some processing time
         std::this_thread::sleep_for(std::chrono::milliseconds(1));
         balance += amount;
       }

       void withdraw(double amount)
       {
        // lock the mutex to ensure exclusive access to the balance
        std::lock_guard<std::mutex> lock(mtx);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (amount <= balance)
        {
            balance -= amount;
        }else
        {
            std::cout << "Insufficient funds" << std::endl;
        }

       }

       double getBalance()
       {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
       }
};

int main()
{
    auto start = std::chrono::high_resolution_clock::now();
    BankAccount account;
    // create multiple threads to deposit and withdraw money.
    std::thread depositThread1([&]{
        for (int i = 0; i < 1000; i++)
        {
            account.deposit(2.0);
        }
    });

    std::thread withdrawThread1([&]{
        for (int i = 0; i < 500; i++)
        {
            account.withdraw(1.0);
        }
    });

    std::thread withdrawThread2([&]{
        for (int i = 0; i < 500; i++)
        {
            account.withdraw(1.0);
        }
    });
  
    depositThread1.join();
    withdrawThread1.join();
    withdrawThread2.join();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Final balance: " << account.getBalance() << std::endl;
    return 0;
}