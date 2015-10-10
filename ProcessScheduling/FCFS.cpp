/************************************************
 * title: FCFS.cpp
 * Description: First come first serve Scheduling.
 * Created on: 10-Oct-2015
 * Author: Rajmani Arya
 ************************************************/
#include <iostream>
#include <algorithm>
#include <vector>
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
 * Comparison b/w processes for sorting
 */
bool comapreArrivalTime(const Process & p1, const Process & p2) {
	if(p1.at == p2.at)
		return p1.pid < p2.pid;
	return p1.at < p2.at;
}
int main() {
	int N;
	cout << "Enter No. of process : "; cin >> N;
	Process p[N];
	for(int i=0; i<N; i++){
		cin >> p[i].pid >> p[i].at >> p[i].bt;
	}
	// Sort process structure a/c to arrival time in case of tie give priority to lesser pid
	sort(p, p+N, comapreArrivalTime);
	p[0].ct = p[0].bt+p[0].at;  // completion time for first process id burst time+arrival time
	for(int i=1; i<N; i++)
		p[i].ct = p[i-1].ct + p[i].bt;   // burst time+previous process completion time
	for(int i=0; i<N; i++){
		p[i].tat = p[i].ct-p[i].at;      // turn around time = ct-at
		p[i].wt = p[i].tat-p[i].bt;      // waiting time = tat-bt
	}
	cout << "\nScheduler Length : " << p[N-1].ct << "\n";
	float avg_tat = 0.0, avg_wt=0.0;
	for(int i=0; i<N; i++) {
		avg_tat += p[i].tat;
		avg_wt += p[i].wt;
	}
	cout << "\nAverage Waiting Time : " << avg_wt/N;
	cout << "\nAverage Turn Around Time : " << avg_tat/N;
	return 0;
}



