#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int completion;
    int burst;
    int turnaround;
    int waiting;
    int remaining;
};

void FCFS(vector<Process> p, int n) {

    sort(p.begin(), p.end(), [] (auto &a, auto &b) {
         return a.arrival < b.arrival;
         });

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival) {
            current_time = p[i].arrival;
        }
        p[i].completion = current_time + p[i].burst;
        p[i].turnaround = p[i].completion - p[i].arrival;
        p[i].waiting = p[i].turnaround - p[i].burst;

        current_time = p[i].completion;
    }

    double total_TAT = 0, total_WT = 0;
    cout << "\nPID \t AT \t CT \t TAT \t TWT \n";
    for (auto &pr: p) {
        cout << pr.pid << "\t" << pr.arrival << "\t" <<pr.completion<< "\t" << pr.turnaround << "\t" <<pr.waiting << "\n";
        total_TAT += pr.turnaround;
        total_WT += pr.waiting;
    }
    cout << "Average turnaround time: " << total_TAT / n << endl;
    cout << "Average Waiting time: " << total_WT / n << endl;
}

int main () {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> l(n);
    for (int i = 0; i < n; i++) {
        cout << "\nEnter Arrival and Burst time for process " << i + 1 << " ";
        cin >> l[i].arrival >> l[i].burst;
        l[i].pid = i + 1;
    }
    FCFS(l, n);



    return 0;

}
