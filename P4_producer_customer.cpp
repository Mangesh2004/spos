//Producer Consumer Problem
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
using namespace std;

const int BUFFER_SIZE = 5; //max. size of the buffer
queue<int> buffer; //shared buffer
mutex mtx;//mutual exclusion
condition_variable not_full, not_empty; //for sync

void producer() {
    int item = 0;
    for (int i = 0; i < 10; i++) {
        unique_lock<mutex> lock(mtx);

        //wait if buffer is full
        not_full.wait(lock, [] {return buffer.size() < BUFFER_SIZE;});

        item++;
        buffer.push(item);
        cout << "Producer Produced: " << item << endl;

        lock.unlock();
        not_empty.notify_one(); //notify a waiting consumer

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void consumer() {
    for (int i = 0; i < 10; i++) {
        unique_lock<mutex> lock(mtx);

        //wait if buffer is empty
        not_empty.wait(lock, [] {return buffer.size() > 0;});

        int item = buffer.front();
        buffer.pop();
        cout << "Consumer consumed: " << item << endl;

        lock.unlock();
        not_full.notify_one(); //notify a waiting producer;

        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main() {
    thread prod(producer);
    thread cons(consumer);

    prod.join();
    cons.join();

    cout << "All items produced and consumed.";
}