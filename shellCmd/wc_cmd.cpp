/**
 * WC unix command
 * prints no. of new lines, no. of words and total bytes
 * of a given file.
 * Author : Rajmani Arya , Date : 10-Sep-2015
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char * argv[]) {
    char pathname[128];
    getcwd(pathname, 128);
    if(argc == 1){
        printf("Usage: <filename>\n");
        exit(1);
    } else {
        if(argc == 2){
            if(access(argv[1], F_OK) != 0){
                printf("File doesn't exist or permission denied !\n");
                exit(2);
            }
            //printf("%s\n", argv[1]);
            int fd=open(argv[1], O_RDONLY); char ch;
            int nl=0,w=0,b=0; int prev = 0;
            while(read(fd, &ch, 1)!=0){
                if(ch == 10) nl++;
                if(!prev){  // if previous char was 9, 32 or 10
                    if(ch == 9 || ch == 32 || ch == 10){
                        prev=1;
                        w++;
                    }else
                        prev=0;
                } else {
                    if(ch == 9 || ch == 32 || ch == 10){
                        prev=1;
                    }else
                        prev=0;
                }
                b++;
            }
            printf("%d %d %d %s", nl, w, b, argv[1]);
        }       
    }
    printf("\n");
    return 0;
}
