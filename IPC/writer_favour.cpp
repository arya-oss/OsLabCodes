#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <stack>

using namespace std;

int TNUMS;

struct rwlock_t {
	pthread_cond_t writeOk, readOk;
	int AR, AW, WR, WW;
	pthread_mutex_t lock;
	rwlock_t() {
		pthread_mutex_init(&lock, NULL);
		AW = AR = WR = WW = 0;
		pthread_cond_init(&writeOk, NULL);
		pthread_cond_init(&readOk, NULL);
	}
	void reader_acquire() {
		pthread_mutex_lock(&lock);
		while(AW || WW > 0) {
			WR++;
			pthread_cond_wait(&readOk, &lock);
			WR--;
		}
		AR++;
		pthread_mutex_unlock(&lock);
	}
	void reader_release() {
		pthread_mutex_lock(&lock);
		AR--;
		if(AR==0 || WW > 0)
			pthread_cond_signal(&writeOk);
		pthread_mutex_unlock(&lock);
	}
	void writer_acquire() {
		pthread_mutex_lock(&lock);
		while(AW || AR > 0) {
			WW++;
			pthread_cond_wait(&readOk, &lock);
			WW--;
		}
		AW = 1;
		pthread_mutex_unlock(&lock);
	}
	void writer_release() {
		pthread_mutex_lock(&lock);
		AW = 0;
		if(WR > 0)
			pthread_cond_broadcast(&readOk);
		else if(WW > 0)
			pthread_cond_signal(&writeOk);
		pthread_mutex_unlock(&lock);
	}
} rwl;

struct Resource {
	stack<int> * s;
	void read(pthread_t id){
		printf("Reader %d reads %d\n", (int)id, s->top());
	}
	void write(pthread_t id){
		printf("Writer %d removes top %d\n", (int)id, s->top());
		s->pop();
	}
};

void * reader(void * args){
	Resource * r = (Resource *) args;
	rwl.reader_acquire();
	pthread_t id=pthread_self();
	r->read(id);
	rwl.reader_release();
}
void * writer(void * args){
	Resource * r = (Resource *) args;
	rwl.writer_acquire();
	pthread_t id=pthread_self();
	r->write(id);
	rwl.writer_release();
}
int main(int argc, char const *argv[])
{	
	if(argc < 2) {
		printf("USAGE : ./a.out <THREAD_NUMS>\n");
		exit(1);
	}
	
	TNUMS = atoi(argv[1]);

	stack<int> * s= new stack<int>();
	for (int i = 0; i <= TNUMS; ++i)
		s->push(i);
	// Filled stack equal to Thread Counts for reading  and removing purpose
	Resource * r = new Resource();
	r->s = s;
	
	pthread_t read_t[TNUMS], write_t[TNUMS];
	for (int i = 0; i < TNUMS; ++i){
		pthread_create(&read_t[i],NULL, &reader, (void *)r);
		pthread_create(&write_t[i],NULL, &writer, (void *)r);
	}
	for (int i = 0; i < TNUMS; ++i) {
		pthread_join(read_t[i], NULL);
	}
	for (int i = 0; i < TNUMS; ++i) {
		pthread_join(write_t[i], NULL);
	}
	exit(EXIT_SUCCESS);
	return 0;
}
