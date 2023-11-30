#include<iostream>
#include<thread>
#include<mutex>
#include<semaphore>
#include<chrono>

std::mutex mtx; // mutex for critical section.
int shared_data = 0; // shared data among threads.

void incrementSharedData(){
    mtx.lock(); // lock the mutex.
    ++shared_data; // Modify the shared data.
    std::cout << "Data incremented by:" << shared_data << std::endl;
    mtx.unlock(); // unlock the mutex.
}

std::counting_semaphore<2> sem(2); // A semaphore that allows 2 concurrent threads.

void accessResource(int threadID){
    sem.acquire(); // wait for access
    std::cout << "Resource accessed by thread " << threadID << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // simulate work.
    std::cout << "Resource released by thread" << threadID << std::endl;
    sem.release(); // release access.
}

int main(){
   // std::thread t1(incrementSharedData);
   // std::thread t2(incrementSharedData);
   // t1.join();
   // t2.join();
   std::thread t1(accessResource, 1);
   std::thread t2(accessResource, 2);
   std::thread t3(accessResource, 3); // this thread will wait until one of the other threads releases the semaphore.
   t1.join();
   t2.join();
   t3.join();
   return 0;
}