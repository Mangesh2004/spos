// mutex.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <array>

using namespace std;
using namespace std::chrono_literals;

namespace ProducerConsumer {
    constexpr int BUFFER_SIZE = 5;
    queue<int> buffer;
    mutex mtx;
    condition_variable not_full, not_empty;

    void producer(int id, int produceCount = 5) {
        for (int i = 0; i < produceCount; ++i) {
            unique_lock<mutex> lock(mtx);

            // Wait until buffer has space
            not_full.wait(lock, [] { return buffer.size() < BUFFER_SIZE; });

            int item = id * 100 + i;
            buffer.push(item);
            cout << "[Producer " << id << "] produced: " << item << '\n';

            lock.unlock();
            not_empty.notify_one();  // notify a waiting consumer
            this_thread::sleep_for(100ms);
        }
    }

    void consumer(int id, int consumeCount = 5) {
        for (int i = 0; i < consumeCount; ++i) {
            unique_lock<mutex> lock(mtx);

            // Wait until buffer has an item
            not_empty.wait(lock, [] { return !buffer.empty(); });

            int item = buffer.front();
            buffer.pop();
            cout << "[Consumer " << id << "] consumed: " << item << '\n';

            lock.unlock();
            not_full.notify_one();  // notify a waiting producer
            this_thread::sleep_for(150ms);
        }
    }

    void run() {
        cout << "\n=== PRODUCER-CONSUMER PROBLEM ===\n";
        thread p1(producer, 1, 5);
        thread p2(producer, 2, 5);
        thread c1(consumer, 1, 5);
        thread c2(consumer, 2, 5);

        p1.join();
        p2.join();
        c1.join();
        c2.join();
    }
}

// ---------------- Readers-Writers ----------------
namespace ReadersWriters {
    int shared_data = 0;
    int read_count = 0;
    mutex read_count_mutex;
    mutex write_mutex;

    void reader(int id, int iterations = 3) {
        for (int i = 0; i < iterations; ++i) {
            // Entry section
            {
                lock_guard<mutex> lock(read_count_mutex);
                ++read_count;
                if (read_count == 1) {
                    write_mutex.lock(); // first reader locks writers out
                }
            }

            // Critical section (reading)
            cout << "[Reader " << id << "] reads: " << shared_data << '\n';
            this_thread::sleep_for(100ms);

            // Exit section
            {
                lock_guard<mutex> lock(read_count_mutex);
                --read_count;
                if (read_count == 0) {
                    write_mutex.unlock(); // last reader allows writers
                }
            }

            this_thread::sleep_for(50ms);
        }
    }

    void writer(int id, int iterations = 2) {
        for (int i = 0; i < iterations; ++i) {
            lock_guard<mutex> lock(write_mutex);
            ++shared_data;
            cout << "[Writer " << id << "] writes: " << shared_data << '\n';
            this_thread::sleep_for(200ms);
            // write_mutex unlocked automatically by lock_guard
            this_thread::sleep_for(100ms);
        }
    }

    void run() {
        cout << "\n=== READERS-WRITERS PROBLEM ===\n";
        thread r1(reader, 1, 3);
        thread r2(reader, 2, 3);
        thread r3(reader, 3, 3);
        thread w1(writer, 1, 2);
        thread w2(writer, 2, 2);

        r1.join();
        r2.join();
        r3.join();
        w1.join();
        w2.join();
    }
}

// ---------------- Dining Philosophers ----------------
namespace DiningPhilosophers {
    constexpr int NUM_PHILOSOPHERS = 5;
    array<mutex, NUM_PHILOSOPHERS> forks;

    void philosopher(int id, int rounds = 3) {
        int left = id;
        int right = (id + 1) % NUM_PHILOSOPHERS;

        for (int r = 0; r < rounds; ++r) {
            // thinking
            cout << "[Philosopher " << id << "] is thinking...\n";
            this_thread::sleep_for(100ms);

            // Pick forks - avoid deadlock by ordering
            if (id % 2 == 0) {
                forks[left].lock();
                forks[right].lock();
            } else {
                forks[right].lock();
                forks[left].lock();
            }

            // eating
            cout << "[Philosopher " << id << "] is eating (round " << r+1 << ")...\n";
            this_thread::sleep_for(150ms);

            // put down forks
            forks[left].unlock();
            forks[right].unlock();

            this_thread::sleep_for(50ms);
        }
    }

    void run() {
        cout << "\n=== DINING PHILOSOPHERS PROBLEM ===\n";
        vector<thread> philosophers;
        for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
            philosophers.emplace_back(philosopher, i, 3);
        }
        for (auto &t : philosophers) t.join();
    }
}

int main() {
    ProducerConsumer::run();
    this_thread::sleep_for(500ms);

    ReadersWriters::run();
    this_thread::sleep_for(500ms);

    DiningPhilosophers::run();
    this_thread::sleep_for(500ms);

    cout << "\nAll problems completed.\n";
    return 0;
}