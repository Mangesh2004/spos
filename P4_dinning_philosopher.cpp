#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
using namespace std;

const int NUM_PHILOSOPHERS = 5;
mutex forks[NUM_PHILOSOPHERS]; 
mutex print_mutex; //  new mutex for synchronized console output

void safe_print(const string &msg) {
    lock_guard<mutex> lock(print_mutex);
    cout << msg << endl;
}

void philosopher(int id) {
    int left = id;
    int right = (id + 1) % NUM_PHILOSOPHERS;

    for (int i = 0; i < 1; i++) {
        safe_print("Philosopher " + to_string(id) + " is thinking");
        this_thread::sleep_for(chrono::milliseconds(500));

        safe_print("Philosopher " + to_string(id) + " is hungry");
        lock(forks[left], forks[right]);

        lock_guard<mutex> left_fork(forks[left], adopt_lock);
        lock_guard<mutex> right_fork(forks[right], adopt_lock);

        safe_print("Philosopher " + to_string(id) + " started eating");
        this_thread::sleep_for(chrono::milliseconds(600));
        safe_print("Philosopher " + to_string(id) + " finished eating");
    }
}

int main() {
    thread philosophers[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i] = thread(philosopher, i);
    }
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers[i].join();
    }
    safe_print("Dinner done");
}
