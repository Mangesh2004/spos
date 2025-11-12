#include <iostream>
#include <shared_mutex>
#include <thread>
#include <chrono>
#include <mutex> // Required for unique_lock and shared_lock

using namespace std;

shared_mutex rw_mutex; // allows multiple readers, single writer
int data_value = 0;

void reader()
{
    for (int i = 0; i < 3; i++)
    {
        shared_lock<shared_mutex> lock(rw_mutex); // shared access (multiple readers)
        cout << "Reader reads data: " << data_value << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void writer()
{
    for (int i = 0; i < 3; i++)
    {
        unique_lock<shared_mutex> lock(rw_mutex); // exclusive access for writer
        data_value += 10;
        cout << "Writer updates data to: " << data_value << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

int main()
{
    thread read_thread(reader);
    thread write_thread(writer);

    read_thread.join();
    write_thread.join();

    cout << "Done" << endl;
    return 0;
}
