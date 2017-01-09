#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]){
    if(argc != 3) exit(1);
    int pid = atoi(argv[1]);
    int i;
    for(i = 0 ; i < 3 ; i++){
        if(argv[2][i] == 'c'){
            kill(pid, SIGINT);
        }
        else if(argv[2][i] == 'z'){
            kill(pid, SIGTSTP);
        }
        else if(argv[2][i] == 'q'){
            kill(pid, SIGQUIT);
        }
        usleep(50);
    }
    return 0;
}