#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

#define key1 123
#define key2 125

typedef struct myMessage {
	long type;
	char msg[140];
}msgq_t;

int main() {
	pid_t pid;
	int sendId = msgget(key2, IPC_CREAT|0666);
	int recId = msgget(key1, IPC_CREAT|0666);
	pid = fork();
	msgq_t m;
	if(pid == 0) {
		char str[140];
		while(1){
			msgrcv(recId, &m, sizeof(m), 0, 0);
			printf("Server: %s\n", m.msg);
		}
	} else if(pid == -1){
		printf("Error in fork\n");
	} else {
		char str[140];
		while(1){
			printf("\nClient: ");
			fscanf(stdin, "%s", str);
			//printf("%s\n",str );
			m.type = getppid();
			strcpy(m.msg, str);
			msgsnd(sendId, &m, sizeof(m), 0);
		}
	}
	return 0;
}