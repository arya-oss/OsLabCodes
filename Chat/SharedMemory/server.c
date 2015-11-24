#include "chat.h"

int main() {
	pid_t pid;
	create_semaphore();
	create_shm_memory();
	sem_init();
	pid = fork();
	if(pid == 0) {
		char * str = attach_shared_memory2();
		while(1){
			sem_wait(1);
			printf("Client: %s\n", str);
		}
	} else if(pid == -1){
		printf("Error in fork\n");
	} else {
		char * str = attach_shared_memory1();
		while(1){
			printf("Server: ");
			scanf("%s", str);
			sem_post(0);
		}
	}
	return 0;
}