#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#define MAX 10

pthread_mutex_t mutex;
pthread_cond_t cndp, cndc;
int buf[MAX];

void * producer(void * args) {
    int * bufindex = (int *) args;
    for(int i=0; i<50 ; i++) {
        pthread_mutex_lock(&mutex);
        if(*bufindex == MAX) {
            printf("Producer waiting.\n");
            pthread_cond_wait(&cndp, &mutex);
        }
        buf[(*bufindex)++] = i;
        printf("producer produced item %d\n", i);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cndc);
    }
}

void * consumer(void * args) {
    int * bufindex = (int *) args;
    for(int i=0; i<50 ; i++) {
        pthread_mutex_lock(&mutex);
        if(*bufindex == -1 ) {
            printf("consumer waiting.\n");
            pthread_cond_wait(&cndc, &mutex);
        }
        printf("consumer consumed item %d\n", buf[(*bufindex)--]);
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
    pthread_create(&prod, NULL, &producer, &bufindex);
    pthread_create(&cons, NULL, &consumer, &bufindex);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
