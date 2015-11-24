#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t table;
pthread_cond_t self;

struct resource {
	int tob, mat, pap, agent;
} * res;

void putdown(int type);
void pickup(int id);

void * agent(void * args) {
	int a,b=0;
	printf("Agent started !\n");
	while(b <10){
		a = rand()%3;
		putdown(a);
		sleep(1);
		b++;
	}
}

void * smoker(void * args) {
	int id = *(int *)args;
	printf("smoker %d started\n", id);
	while(1){
		pickup(id);
		sleep(1);
	}
}

int main() {
	res = (struct resource *)malloc(sizeof(struct resource));
	res->agent=1; res->mat=0; res->pap=0; res->tob=0;
	pthread_t p1, smk[3];
	int i; int smk_id[]= {1,2,3};

	pthread_mutex_init(&table, NULL);
	
	pthread_cond_init(&self, NULL);

	pthread_create(&p1, NULL, &agent, NULL);
	for(i=0; i<3; i++)
		pthread_create(&smk[i], NULL, &smoker, &smk_id[i]);
	pthread_join(p1, NULL);
	for (i = 0; i < 3; ++i){
		pthread_join(smk[i], NULL);
	}
	return 0;
}

void putdown(int type){
	pthread_mutex_lock(&table);
	while(res->agent != 1){
		pthread_cond_wait(&self, &table);
	}
	switch(type){
		case 0: // tobaco
			res->tob=1;
			printf("agent put matches and paper on table\n");
			break;
		case 1: // matches
			res->mat=1;
			printf("agent put matches and paper on table\n");
			break;
		case 2: // paper
			res->pap=1;
			printf("agent put matches and paper on table\n");
			break;
		default:
			break;
	}
	pthread_cond_broadcast(&self);
	pthread_mutex_unlock(&table);
}

void pickup(int id){
	pthread_mutex_lock(&table);
	switch(id){
		case 1:
			while(res->tob!=1){
				pthread_cond_wait(&self, &table);
			}
			res->tob=0;
			res->agent=1;
			printf("smoker %d took tobacco and smoking\n", id);
		break;
		case 2:
			while(res->mat!=1){
				pthread_cond_wait(&self, &table);
			}
			res->mat = 0;
			res->agent=1;
			printf("smoker %d took matches and smoking\n", id);
		break;
		case 3:
			while(res->pap!=1){
				pthread_cond_wait(&self, &table);
			}
			res->pap = 0;
			res->agent = 1;
			printf("smoker %d took paper and smoking\n", id);
		break;
		default:
		break;
	}
	pthread_cond_broadcast(&self);
	pthread_mutex_unlock(&table);
}
