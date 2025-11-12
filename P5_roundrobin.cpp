#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath> // For std::max (used to jump the time)
using namespace std;

// ============================================================
// 🧱 Structure to hold details of each process
// ============================================================
struct Process {
    int pid;        // Process ID
    int at;         // Arrival Time
    int bt;         // Burst Time (CPU time required)
    int ct;         // Completion Time
    int tat;        // Turnaround Time = CT - AT
    int wt;         // Waiting Time = TAT - BT
    int remaining;  // Remaining Burst Time (for preemption)
};

// ============================================================
// 📊 Function to display the process table
// ============================================================
void display(const vector<Process> &p) {
    cout << "\nPID\tAT\tBT\tCT\tTAT\tWT\n";
    for (const auto &x : p)
        cout << "P" << x.pid << "\t"
             << x.at << "\t"
             << x.bt << "\t"
             << x.ct << "\t"
             << x.tat << "\t"
             << x.wt << "\n";
}

// ============================================================
// 📈 Function to calculate and display average times
// ============================================================
void average(const vector<Process> &p) {
    float avg_tat = 0, avg_wt = 0;
    for (const auto &x : p) {
        avg_tat += x.tat;
        avg_wt += x.wt;
    }
    cout << "Average Turnaround Time: " << avg_tat / p.size() << endl;
    cout << "Average Waiting Time: " << avg_wt / p.size() << endl;
}

// ============================================================
// ⚙️ ROUND ROBIN Scheduling Algorithm (Preemptive)
// ============================================================
void RoundRobin(vector<Process> &p, int quantum) {
    cout << "\n====== Round Robin (Preemptive) ======\n";

    int n = p.size();     // Total number of processes

    // ------------------------------------------------------------
    // Step 1️⃣: Sort all processes by Arrival Time (AT)
    // ------------------------------------------------------------
    sort(p.begin(), p.end(), [](const Process &a, const Process &b) {
        return a.at < b.at;  // earlier arrival = higher priority
    });

    // ------------------------------------------------------------
    // Step 2️⃣: Initialize variables
    // ------------------------------------------------------------
    queue<int> q;          // Ready queue — stores process indices
    int time = 0;          // Tracks the CPU clock
    int completed = 0;     // Counts how many processes finished
    vector<bool> inQ(n, false);  // Keeps track of which processes are in queue
    int last_arrival_check = 1;  // Index for checking new arrivals

    // Initialize remaining burst time for each process
    for (auto &x : p)
        x.remaining = x.bt;

    // ------------------------------------------------------------
    // Step 3️⃣: Start scheduling
    // ------------------------------------------------------------

    // Initially, set time to the first process's arrival
    time = p[0].at;

    // Push the first process to the ready queue
    q.push(0);
    inQ[0] = true;

    // ------------------------------------------------------------
    // Main scheduling loop — runs until all processes complete
    // ------------------------------------------------------------
    while (completed < n) {

        // 🕓 If queue is empty (no ready process),
        // jump the CPU clock to the next process arrival time.
        if (q.empty()) {
            for (int i = 0; i < n; i++) {
                if (p[i].remaining > 0) {          // Find the next process not done
                    time = max(time, p[i].at);     // Jump forward in time
                    q.push(i);                     // Add to queue
                    inQ[i] = true;
                    last_arrival_check = i + 1;    // Update next arrival index
                    break;
                }
            }
        }

        // 🧩 Dequeue (pop) the first process from ready queue
        int idx = q.front();
        q.pop();
        inQ[idx] = false;   // Mark it as not in queue

        // ⚙️ Execute process for 'quantum' or less if it finishes earlier
        int exec = min(quantum, p[idx].remaining);

        // Deduct the executed time from its remaining time
        p[idx].remaining -= exec;

        // Move CPU time forward
        time += exec;

        // ------------------------------------------------------------
        // Step 4️⃣: Add newly arrived processes to the ready queue
        // ------------------------------------------------------------
        // Because processes are sorted by arrival,
        // check only from the last arrival index we’ve seen.
        while (last_arrival_check < n && p[last_arrival_check].at <= time) {
            if (p[last_arrival_check].remaining > 0 && !inQ[last_arrival_check]) {
                q.push(last_arrival_check);
                inQ[last_arrival_check] = true;
            }
            last_arrival_check++;
        }

        // ------------------------------------------------------------
        // Step 5️⃣: If process finished, mark completion details
        // ------------------------------------------------------------
        if (p[idx].remaining == 0) {
            // Process has fully executed
            p[idx].ct = time;                       // Completion Time
            p[idx].tat = p[idx].ct - p[idx].at;     // Turnaround = CT - AT
            p[idx].wt = p[idx].tat - p[idx].bt;     // Waiting = TAT - BT
            completed++;                            // Increase count of finished processes
        } else {
            // Process not finished — push it back to queue (preempted)
            q.push(idx);
            inQ[idx] = true;
        }
    }

    // ------------------------------------------------------------
    // Step 6️⃣: Display final results
    // ------------------------------------------------------------
    display(p);
    average(p);
}

// ============================================================
// 🧭 MAIN FUNCTION — Input & start scheduling
// ============================================================
int main() {
    int n, q;

    // Ask user for number of processes
    cout << "Enter number of processes: ";
    if (!(cin >> n) || n <= 0) {
        cerr << "Invalid number of processes.\n";
        return 1;
    }

    // Vector to hold all process data
    vector<Process> p(n);

    // Input loop for arrival & burst times
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        cout << "Enter AT and BT for P" << i + 1 << ": ";
        if (!(cin >> p[i].at >> p[i].bt) || p[i].at < 0 || p[i].bt <= 0) {
            cerr << "Invalid AT or BT for P" << i + 1 << ".\n";
            return 1;
        }
    }

    // Ask user for Time Quantum
    cout << "Enter Time Quantum: ";
    if (!(cin >> q) || q <= 0) {
        cerr << "Invalid time quantum.\n";
        return 1;
    }

    // Run the Round Robin scheduler
    RoundRobin(p, q);

    return 0;
}
