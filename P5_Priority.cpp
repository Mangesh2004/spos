#include <iostream>
#include <vector>
using namespace std;

struct Process {
    int pid, arrival, completion, burst, turnaround, waiting;
    int priority;
    bool done = false;
};

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);

    for (int i = 0; i < n; i++) {
        cout << "\nEnter arrival and burst for process " << i+1 << " ";
        cin >> p[i].arrival >> p[i].burst;
        cout << "\nEnter priority of process " << i+1 << ": ";
        cin >> p[i].priority;
        p[i].pid = i+1;
    }
    int completed = 0;
    int current_time = 0;
    int total_TAT;
    int total_WT;


    while (completed < n) {
        int idx = -1;
        int bestPriority = 65532;

        for (int i = 0; i < n; i++) {
            if (p[i].done == false && p[i].arrival <=current_time) {
                if (p[i].priority < bestPriority) {
                    bestPriority = p[i].priority;
                    idx = i;
                }
            }
        }
        if (idx == -1) {
            current_time++;
            continue;
        }

        //execute selected process completely.
        current_time += p[idx].burst;
        p[idx].completion = current_time;
        p[idx].turnaround = current_time - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        p[idx].done = true;
        completed++;

        cout << "\nProcess " <<p[idx].pid << " ";
        total_TAT = p[idx].turnaround;
        total_WT = p[idx].waiting;
    }

    cout << "\n\nPID\tAT\tBT\tPR\tCT\tTAT\tWT\n";
    for (auto &pr : p) {
        cout << pr.pid << "\t" << pr.arrival << "\t" << pr.burst
             << "\t" << pr.priority << "\t" << pr.completion
             << "\t" << pr.turnaround << "\t" << pr.waiting << "\n";
    }

    cout << "\nAverage Turnaround Time: " << total_TAT / n;
    cout << "\nAverage Waiting Time: " << total_WT / n;
}
