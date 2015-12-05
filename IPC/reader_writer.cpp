#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>

#define TNUMS 5

struct rwlock_t {
	sem_t w_lock;
	sem_t r_lock;
	int readers;
	rwlock_t() {
		readers=0;
		sem_init(&w_lock, 0, 1);
		sem_init(&r_lock, 0, 1);
	}
	void reader_acquire() {
		sem_wait(&r_lock);
		readers++;
		if(readers==1)
			sem_wait(&w_lock);
		sem_post(&r_lock);
	}
	void reader_release() {
		sem_wait(&r_lock);
		readers--;
		if(readers==0)
			sem_post(&w_lock);
		sem_post(&r_lock);
	}
	void writer_acquire() {
		sem_wait(&w_lock);
	}
	void writer_release() {
		sem_post(&w_lock);
	}
} rwl;

void * reader(void * args){
	rwl.reader_acquire();
	pthread_t id=pthread_self();
	printf("Reader %d is reading\n",(int)id);
	rwl.reader_release();
}
void * writer(void * args){
	Resource * r = (Resource *) args;
	rwl.writer_acquire();
	pthread_t id=pthread_self();
	printf("Writer %d is writing\n",(int)id);
	rwl.writer_release();
}
int main(int argc, char const *argv[])
{	
	pthread_t red_t[TNUMS], writ_t[TNUMS];
	for (int i = 0; i < TNUMS; ++i){
		pthread_create(&red_t[i],NULL, &reader, NULL);
		pthread_create(&writ_t[i],NULL, &writer, NULL);
	}
	for (int i = 0; i < TNUMS; ++i) {
		pthread_join(red_t[i], NULL);
		pthread_join(writ_t[i], NULL);
	}
	return 0;
}
