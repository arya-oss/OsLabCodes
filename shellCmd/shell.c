#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#define i_redirect 1
#define o_redirect 2
void get_args (char * buffer, char * argv[], int * nargs) {
	char * s = strtok(buffer, " \t\n");
	*nargs=0;
	while(s != NULL){
		argv[*nargs] = s;
		*nargs = *nargs+1;
		s = strtok(NULL, " \t\n");
	}
	argv[*nargs] = NULL;
}

int main(int argc, char const *argv[]){
	char buffer[1024];
	char * args[64];
	int * status;
	int nargs;
	int flag; char filename[128];
	while(1) {
		printf("\nRajmani@Arya $ ");
		flag=0;
		fgets(buffer, 1024, stdin);
		get_args(buffer, args, &nargs);
		if(nargs == 0) continue;
		/*if(*(args+nargs-1)[0] == '>'){
			flag = 2;
			memcpy(filename, *(args+nargs-1)+1, strlen(*(args+nargs-1))-1);
			filename[strlen(*(args+nargs-1))-1]='\0';
		} else if(*(args+nargs-1)[0] == '<'){
			flag = 2;
			memcpy(filename, *(args+nargs-1)+1, strlen(*(args+nargs-1))-1);
			filename[strlen(*(args+nargs-1))-1]='\0';
		}*/
		if(!strcmp(args[0], "exit"))
			exit(0);
		if(fork()) {
			waitpid(status);
			printf("\ncommand exec finished\n");
		} else {
			/*if(flag1){
				int fd = open(filename1, "O_RDONLY");
				if(fd != -1)
					dup2(0, fd)
			}
			if(flag2){
				int fd = open(filename2, "O_WRONLY");
				if(fd != -1)
					dup2(1, fd)
			}*/
			execvp(args[0], args)
			printf("Error occured !\n");
		}
	}
	return 0;
}