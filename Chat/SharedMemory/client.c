#include "chat.h"

int main() {
	pid_t pid;
	create_semaphore();
	create_shm_memory();
	pid = fork();
	if(pid == 0) {
		char * str = attach_shared_memory1();
		while(1){
			sem_wait(0);
			printf("Server: %s\n", str);
		}
	} else if(pid == -1){
		printf("Error in fork\n");
	} else {
		char * str = attach_shared_memory2();
		while(1){
			printf("Client: ");
			scanf("%s", str);
			sem_post(1);
		}
	}
	return 0;
}