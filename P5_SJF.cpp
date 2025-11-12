#include <iostream>
#include <vector>
using namespace std;

struct Process {
    int pid;
    int arrival;
    int completion;
    int burst;
    int turnaround; //completion - arrival
    int waiting; //turnaround - burst
    int remaining;
};

int main() {
    int n;
    cout <<"\nEnter number of processes: ";
    cin >> n; cout << endl;

    vector<Process> p(n);

    for (int i = 0; i < n; i++) {
        cout << "Enter arrival and burst time for process number " << i + 1 << ": ";
        cin >> p[i].arrival >> p[i].burst;
        p[i].pid = i+1;
        p[i].remaining = p[i].burst;
    }
    int completed = 0;
    vector<int> done(n);
    for (int i = 0; i < n; i++) {
        done[i] = 0;
    }
    int current_time = 0;
    int total_TAT = 0;
    int total_WT = 0;

    while (completed < n) {
        int idx = -1;
        int min_remaining = 65534;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival <= current_time && done[i] == 0 && p[i].remaining < min_remaining && p[i].remaining > 0) {
                    min_remaining = p[i].remaining;
                    idx = i;
                }
        }
        if (idx == -1) { //no process yet
            current_time++;
            continue;
        }
        p[idx].remaining--;

        cout << "Time: " << current_time << "Process " << p[idx].pid << "\n";
        current_time++;

        //if process complete
        if (p[idx].remaining == 0) {
            done[idx] = 1;
            completed++;
            p[idx].completion = current_time;
            p[idx].turnaround = current_time - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;

            total_TAT += p[idx].turnaround;
            total_WT += p[idx].waiting;
        }
    }

    cout << "\nPID \t AT \t CT \t BT \t TAT \t TWT \n";
    for (auto &pr: p) {
        cout << pr.pid << "\t" << pr.arrival << "\t";
        cout << pr.completion << "\t" << pr.burst << "\t";
        cout << pr.turnaround << "\t" << pr.waiting << "\n";
    }
    cout << "Average TAT: " << total_TAT / n << "\n";
    cout << "Average WT: " << total_WT / n << "\n";
}