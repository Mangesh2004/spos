#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;

struct Process
{
    int pid;
    int at;  // Arrival Time
    int bt;  // Burst Time
    int pr;  // Priority
    int ct;  // Completion Time
    int tat; // Turnaround Time
    int wt;  // Waiting Time
    int rt;  // Remaining Time

};

// Function to display the result table
void display(vector<Process> &p)
{
    cout << "\nPID\tAT\tBT\tPR\tCT\tTAT\tWT\n";
    for (auto &x : p)
        cout << "P" << x.pid << "\t" << x.at << "\t" << x.bt << "\t" << x.pr
             << "\t" << x.ct << "\t" << x.tat << "\t" << x.wt << "\n";
}

// Function to calculate averages
void average(vector<Process> &p)
{
    float avg_tat = 0, avg_wt = 0;
    for (auto &x : p)
    {
        avg_tat += x.tat;
        avg_wt += x.wt;
    }
    cout << "\nAverage Turnaround Time: " << avg_tat / p.size();
    cout << "\nAverage Waiting Time: " << avg_wt / p.size() << "\n";
}

/*---------------------------------------------
    FCFS
---------------------------------------------*/
void FCFS(vector<Process> p)
{
    cout << "\n=== FCFS Scheduling ===\n";
    sort(p.begin(), p.end(), [](Process a, Process b)
         { return a.at < b.at; });

    int time = 0;
    for (auto &x : p)
    {
        if (time < x.at)
            time = x.at;
        time += x.bt;
        x.ct = time;
        x.tat = x.ct - x.at;
        x.wt = x.tat - x.bt;
    }

    display(p);
    average(p);
}

/*---------------------------------------------
   SJF (Preemptive)
---------------------------------------------*/
void SJF_Preemptive(vector<Process> p)
{
    cout << "\n=== SJF (Preemptive) ===\n";
    int n = p.size(), completed = 0, time = 0;
    for (auto &x : p)
        x.rt = x.bt;

    while (completed != n)
    {
        int idx = -1, min_rt = 1e9;
        for (int i = 0; i < n; i++)
        {
            if (p[i].at <= time && p[i].rt > 0 && p[i].rt < min_rt)
            {
                min_rt = p[i].rt;
                idx = i;
            }
        }

        if (idx == -1)
        {
            time++;
            continue;
        }

        p[idx].rt--;
        if (p[idx].rt == 0)
        {
            completed++;
            p[idx].ct = time + 1;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
        }
        time++;
    }

    display(p);
    average(p);
}

/*---------------------------------------------
   Priority (Non-Preemptive)
---------------------------------------------*/
void Priority(vector<Process> p)
{
    cout << "\n=== Priority (Non-Preemptive) ===\n";
    int n = p.size(), completed = 0, time = 0;
    vector<bool> done(n, false);

    while (completed != n)
    {
        int idx = -1, best = 1e9;
        for (int i = 0; i < n; i++)
        {
            if (!done[i] && p[i].at <= time && p[i].pr < best)
            {
                best = p[i].pr;
                idx = i;
            }
        }

        if (idx == -1)
        {
            time++;
            continue;
        }

        time += p[idx].bt;
        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        done[idx] = true;
        completed++;
    }

    display(p);
    average(p);
}

/*---------------------------------------------
    Round Robin
---------------------------------------------*/
void RoundRobin(vector<Process> &p, int quantum) {
    cout << "\n====== Round Robin (Preemptive) ======\n";
    int n = p.size();
    
    // Sort processes by Arrival Time (AT)
    sort(p.begin(), p.end(), [](const Process &a, const Process &b) {
        return a.at < b.at; 
    });

    queue<int> q; // Ready queue, holds indices of processes in p
    int time = 0;
    int completed = 0;
    vector<bool> inQ(n, false);
    int last_arrival_check = 1; // Start checking from the second process (index 1)

    // Initialize remaining burst time
    for (auto &x : p)
        x.rt = x.bt;

    // Start with the first arrived process
    time = p[0].at;
    q.push(0);
    inQ[0] = true;

    while (completed < n) {
        // If the queue is empty, advance time to the arrival of the next process
        if (q.empty()) {
            for (int i = 0; i < n; i++) {
                if (p[i].rt > 0) {
                    time = max(time, p[i].at); // Jump time to the next arrival
                    q.push(i);
                    inQ[i] = true;
                    // Update last_arrival_check to ensure we don't re-add this on the next check
                    last_arrival_check = i + 1; 
                    break;
                }
            }
        }

        int idx = q.front();
        q.pop();
        inQ[idx] = false;

        int exec = min(quantum, p[idx].rt);
        p[idx].rt -= exec;
        time += exec;

        // --- Step 1: Add newly arrived processes ---
        // Processes are sorted by AT, so we only need to check from last_arrival_check
        while (last_arrival_check < n && p[last_arrival_check].at <= time) {
            if (p[last_arrival_check].rt > 0 && !inQ[last_arrival_check]) {
                q.push(last_arrival_check);
                inQ[last_arrival_check] = true;
            }
            last_arrival_check++;
        }
        
        // --- Step 2: Handle completion or preemption ---
        if (p[idx].rt == 0) {
            // Process completed
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            completed++;
        } else {
            // Process preempted, add back to the end of the queue
            q.push(idx);
            inQ[idx] = true;
        }
    }

    display(p);
    average(p);
}

/*---------------------------------------------
   Main Function
---------------------------------------------*/
int main()
{
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++)
    {
        p[i].pid = i + 1;
        cout << "Enter AT BT PR for P" << i + 1 << ": ";
        cin >> p[i].at >> p[i].bt >> p[i].pr;
    }

    FCFS(p);
    SJF_Preemptive(p);
    Priority(p);

    int q;
    cout << "\nEnter Time Quantum for Round Robin: ";
    cin >> q;
    RoundRobin(p, q);

    return 0;
}
