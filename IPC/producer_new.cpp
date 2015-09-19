#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <queue>

#define TNUMS 10
#define qSize 5

pthread_mutex_t mutex;
pthread_cond_t cnd;

struct resource {
	std::queue<int> q;
	void enqueue(int data){
		q.push(data);
	}
	int dequeue() {
		int val = q.front();
		q.pop();
		return val;
	}
	int size(){
		return q.size();
	}
	bool empty() {
		return q.empty();
	}
};

void * run_producer(void * args) {
	resource * r = (resource *) args;
	pthread_mutex_lock(&mutex);
	pthread_t pid = pthread_self();
	while(r->size() == qSize){
		printf("Buffer is Full producer %d waiting.\n", pid);
		pthread_cond_wait(&cnd, &mutex);
	}
	int x = pid;
	r->enqueue(x);
	printf("producer %d produced %d .\n", pid, pid);
	pthread_cond_broadcast(&cnd);
	pthread_mutex_unlock(&mutex);
}

void * run_consumer(void * args) {
	resource * r = (resource *) args;
	pthread_mutex_lock(&mutex);
	pthread_t pid = pthread_self();
	while(r->size() == 0){
		printf("Buffer is empty consumer %d waiting.\n", pid);
		pthread_cond_wait(&cnd, &mutex);
	}
	int x = r->dequeue();
	printf("Consumer %d consumed %d .\n", pid, x);
	pthread_cond_broadcast(&cnd);
	pthread_mutex_unlock(&mutex);
}

int main(int argc, char const *argv[]){
	resource * r;
	r = new resource();
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cnd, NULL);
	pthread_t prod[TNUMS], cons[TNUMS];
	for (int i = 0; i < TNUMS; ++i){
		if(pthread_create(&prod[i], NULL, &run_producer, (void *) r) !=0 ){
			printf("Error !\n");
			exit(1);
		}
		if(pthread_create(&cons[i], NULL, &run_consumer, (void *) r) !=0 ){
			printf("Error !\n");
			exit(1);
		}
	}
	for (int i = 0; i < TNUMS; ++i) {
		pthread_join(prod[i], NULL);
	}
	for (int i = 0; i < TNUMS; ++i) {
		pthread_join(cons[i], NULL);
	}
	return 0;
}
