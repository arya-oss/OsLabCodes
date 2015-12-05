#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#define MAX 10

pthread_mutex_t mutex;
pthread_cond_t cndp, cndc;
int buf[MAX];

void * producer(void * args) {
    int in=0;
    for(int i=0; i<50 ; i++) {
        pthread_mutex_lock(&mutex);
        while(in == MAX) {
            printf("Producer waiting.\n");
            pthread_cond_wait(&cndp, &mutex);
        }
        buf[in] = i;
        in =(in+1)%MAX;
        printf("producer produced item %d\n", i);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cndc);
    }
}

void * consumer(void * args) {
    int out=0;
    for(int i=0; i<50 ; i++) {
        pthread_mutex_lock(&mutex);
        while(out == 0 ) {
            printf("consumer waiting.\n");
            pthread_cond_wait(&cndc, &mutex);
        }
        printf("consumer consumed item %d\n", buf[out]);
        out =(out+1)%MAX;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cndp);
    }
}


int main() {
    int bufindex=0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cndc, NULL);
    pthread_cond_init(&cndp, NULL);
    pthread_t cons, prod;
    pthread_create(&prod, NULL, &producer, NULL);
    pthread_create(&cons, NULL, &consumer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
