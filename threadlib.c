#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <malloc.h>
#include <signal.h>

#define MAX_THREADS 10
#define MEM 64000 //memory allocated to each thread
#define INTERVAL 50
struct mythread_t{
	int id;
	ucontext_t uc;
	void *retval;
	int completed;
};


// queue for holding the contexts for round robin scheduling 
struct queue{
	int size ;
	struct mythread_t **elements;
	int front;
	int rear;
};
struct queue *readyq;

struct mythread_t *maint;
struct mythread_t *current;
struct itimerval timer;

void initqueue(struct queue *q){
	q->size = MAX_THREADS;
	q->elements = (struct mythread_t**)malloc(sizeof(struct mythread_t*)*q->size);
	q->front = -1;
	q->rear = -1;
}

int isEmpty(struct queue *q){
	if (q->front == -1 && q->rear == -1){
		return 1;
	}
	return 0;
}


int isFull(struct queue *q){
	if((q->rear+1)%q->size == q->front){
		return 1;
	}
	return 0;
}
int enqueue(struct queue *q, struct mythread_t *t){
	if(isFull(q)){
		printf("Queue is full\n");
		return;
	}
	if(isEmpty(q)){
		q->front = 0;

	}
	q->rear = (q->rear+1)%q->size;	
	q->elements[q->rear] = t;
	return q->rear;

}


struct mythread_t* dequeue(struct queue *q){
	if (isEmpty(q)){
		printf("Queue is empty\n");
		exit(1);
	}
	struct mythread_t *t = q->elements[q->front];
	// q.elements[q.front] = NULL;
	if(q->front == q->rear){
		q->front = -1;
		q->rear = -1;
	}
	else{
		q->front = (q->front +1)%q->size;
	}
	return t;
}


// queue operations over



// scheduler operations
//
//
//

void fcfs(){
	while(!isEmpty(readyq)){
		struct mythread_t *to_run = dequeue(readyq);
		current = to_run;
		struct mythread_t *prev = to_run;
		swapcontext(&(maint->uc), &(to_run->uc));
		printf("completed a thread");	
	}
}
void start_time();
void stop_time();

void roundrobin(){

	while(!isEmpty(readyq)){
		
		struct mythread_t *to_run = dequeue(readyq);
		current = to_run;
		struct mythread_t *prev = to_run;
		start_time();
		swapcontext(&(maint->uc), &(to_run->uc));
		stop_time();
		printf("completed a run of roundrobin \n");
		if(!current->completed){
			enqueue(readyq,current);
		}
			
	}


}



void sighandler(int signo){
	swapcontext(&(current->uc), &(maint->uc));	
}

/*
void schedule_init(){
	if(signal(SIGVTALRM, fcfs) == SIG_ERR){
		printf("%s\n","error in creating scheduler");
		exit(1);
	}
}
*/
void start_time(){
	if(setitimer(ITIMER_PROF,  &timer,0) == -1){
		printf("error calling start time");
		exit(1);
	}
}

void stop_time(){
	
	if(setitimer(ITIMER_PROF,  0,0) == -1){
		printf("error calling stop time");
		exit(1);
	}
}

void timer_init(){
	timer.it_value.tv_sec = INTERVAL/1000;
	timer.it_value.tv_usec = (INTERVAL*1000) %1000000;
	//timer.it_value.tv_sec = 0;
	//timer.it_value.tv_usec = 500;
	timer.it_interval = timer.it_value;

	if(signal(SIGPROF, sighandler) == SIG_ERR){
		printf("%s\n","error in creating scheduler");
		exit(1);
	}
}





/*struct mythread_t *maint;
struct mythread_t *current;
struct mythread_t *maint;
struct mythread_t *current;
*/

void mythread_init(){
	maint = (struct mythread_t*)malloc(sizeof(struct mythread_t));
	if (getcontext(&(maint->uc)) == -1){
		printf("Error getting context\n");
		exit(1);
	}
	current = maint;
	readyq = (struct queue*) malloc(sizeof(struct queue)); 
	initqueue(readyq);
}

int mythread_create(struct mythread_t * thread, void *(*start_routine)(void *), void* arg){

	// (*start_routine)(arg);
	// ucontext_t thread->uc;
	if (getcontext(&(thread->uc)) == -1){
		printf("Error getting context\n");
		exit(1);
	}
	//
	// thread->id = enqueue(readyq,*(thread));
	thread->id = rand()%100;
	thread->uc.uc_link = &(maint->uc);
	thread->uc.uc_stack.ss_sp = malloc(MEM);
	thread->uc.uc_stack.ss_size = MEM;
	thread->uc.uc_stack.ss_flags = 0;
	makecontext(&(thread->uc), (void (*) ())start_routine, 1, (void *)&thread->id);
	//making the current thread point to thread about to start running
	current = thread;
	enqueue(readyq,thread);
	//swap context to run the thread
	/*if(swapcontext(&(maint->uc), &(thread->uc )) == -1){
		printf("swap context failed\n");
	}
	printf("Thread %d completed.\n", thread->id);
	*/
	return 0;
}



void mythread_exit(int retval){
	void *r = (void *) &retval;
 	current->retval = r;
	current->completed = 1;
}





void* test(void* arg){
	printf("This is the %d test thread\n", *(int *)arg);
	int i;
	for(i =0; i<500;i++){
		printf("thread1 %d\n", i);
		int j;
		for(j = 0; j<1000000;j++);	
	}
	mythread_exit(1);
}

void* test2(void* arg){
	printf("This is the %d test thread\n", *(int *)arg);
//	sleep(5);
	int i;	
	for(i =0; i<500;i++){
		printf("thread2 %d\n", i);
		
		int j;
		for(j = 0; j<1000000;j++);	
	}
	mythread_exit(1);
}



int main(){
	mythread_init();
	timer_init();
	struct mythread_t t1, t2;
	mythread_create(&t1, test, 0);
	mythread_create(&t2, test2, 0);
	roundrobin();
	printf("all threads completed\n");
}