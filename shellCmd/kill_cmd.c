#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc,char *argv[]) {
    if(argc == 1){
        printf("Usage: ./a.out pid or -s pid\n");
        exit(1);
    }
    if(argc==2) {
        pid_t pid=atoi(argv[1]);
        if(kill(pid,SIGTERM) == -1){
            printf("Error occured !!\n");
        } else
            printf("%s terminated\n", argv[1]);
    }
    else {
        pid_t pid=atoi(argv[2]);
        int sig = -1*atoi(argv[1]);
        if(kill(pid, sig) == -1){
            printf("Error occured !!\n");
        } else {
            printf("%s killed\n", argv[2]);
        }
    }
    return 0;
}
