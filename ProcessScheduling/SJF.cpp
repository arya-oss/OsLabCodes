/************************************************
 * title: SJF.cpp
 * Description: Shortest Job First Scheduling.
 * Created on: 10-Oct-2015
 * Author: Rajmani Arya
 ************************************************/
#include <bits/stdc++.h>

using namespace std;
/**
 * Process structure
 * Pid => process id
 * at => Arrival Time
 * bt => Burst Time
 * wt => waiting time
 * tat => Turn around time
 * ct => completion time
 */
struct Process {
    int pid;
    unsigned int at, bt, tat, wt, ct;
    Process() {
        pid = at = bt = ct = wt = tat = 0;
    }
    Process(int pid, int at, int bt) {
        this->pid = pid;
        this->at = at;
        this->bt = bt;
        tat = ct = wt = 0;
    }
};
/**
 * comparison b/w process in ready queue a/c to burst time
 */
struct Comparison : public binary_function<Process *, Process *, bool> {
    bool operator () (const Process * p1, const Process * p2) {
        if(p1->bt == p2->bt)
            p1->at > p2->at;
        return p1->bt > p2->bt;
    }
};
/**
 * Comparison b/w processes for sorting
 */
bool comapreArrivalTime(const Process & p1, const Process & p2) {
    if(p1.at == p2.at)
        return p1.pid < p2.pid;
    return p1.at < p2.at;
}

int main() {
    int N, offset=0, schedulerTime=0; Process P[100];
    
    priority_queue<Process*, vector<Process*>, Comparison> ready_queue;  // min heap for min burst time process
    
    cout << "Enter No. of process : "; cin >> N;
    for(int i=0; i<N; i++)
        cin >> P[i].pid >> P[i].at >> P[i].bt;
    
    // Sort process structure a/c to arrival time in case of tie give priority to lesser pid
    sort(P, P+N, comapreArrivalTime);
    schedulerTime = P[0].at; ready_queue.push(P); offset=1;
    /**
     * Push initial process for same arrival time in ready queue
     */
    while(offset < N && P[offset].at == schedulerTime){
        ready_queue.push(P+offset);
        offset++;
    }
    // Main Code Block
    while(!ready_queue.empty()) {
        Process * p = ready_queue.top();
        ready_queue.pop();
        p->ct = schedulerTime+p->bt;
        p->tat = p->ct-p->at;
        p->wt = p->tat-p->bt;
        schedulerTime += p->bt;
        while(offset < N && P[offset].at <= schedulerTime) {
            ready_queue.push(P+offset);
            offset++;
        }
    }

    cout << "\nScheduler Length : " << schedulerTime-P[0].at << "\n";
    float avg_tat = 0.0, avg_wt=0.0;
    for(int i=0; i<N; i++) {
        avg_tat += P[i].tat;
        avg_wt += P[i].wt;
    }
    cout << "\nAverage Waiting Time : " << avg_wt/N;
    cout << "\nAverage Turn Around Time : " << avg_tat/N;
    return 0;
}
