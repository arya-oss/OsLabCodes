/************************************************
 * title: RR.cpp
 * Description: Round Robin Scheduling.
 * In case of arrival of new process and ending of slot of
 * previous process priority is given to new process
 * Created on: 10-Oct-2015
 * Author: Rajmani Arya
 ************************************************/
#include <bits/stdc++.h>

using namespace std;

struct Process {
    int pid;
    unsigned int at, bt, tat, wt, ct, rt;
    Process() {
        pid = at = bt = ct = wt = tat = rt = 0;
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
    int N, TQ, offset=0, schedulerTime=0; Process P[100];
    queue<Process *> ready_queue;
    cout << "Enter No. of process : "; cin >> N;
    cout << "Enter Time Quantum : "; cin >> TQ;
    for(int i=0; i<N; i++){
        cin >> P[i].pid >> P[i].at >> P[i].bt;
        P[i].rt = P[i].bt;
    }
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
        Process * p = ready_queue.front();
        ready_queue.pop();
        if(p->rt > TQ){ // process remaining time is greater than Time Quantum
            p->rt -= TQ;
            schedulerTime += TQ;
            while(offset < N && P[offset].at <= schedulerTime){
                ready_queue.push(P+offset);
                offset++;
            }
            ready_queue.push(p);
        } else { // process is completed
            schedulerTime += p->rt; 
            p->rt = 0;
            p->ct = schedulerTime;
            p->tat = p->ct - p->at;
            p->wt = p->tat - p->bt;
            while(offset < N && P[offset].at <= schedulerTime){
                ready_queue.push(P+offset);
                offset++;
            }
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
