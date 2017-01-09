#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int knocked;
int prev_sig = -1;
int caughtSignal = 0;

time_t t_s = -1;
time_t t_e;
sigset_t block_mask;

void handler(int signum);

int install_handler(int signum, void (*handler)(int));

int read_and_echo();

int main(){
    if(install_handler(SIGINT, &handler) < 0) perror("SIGINT\n");
    if(install_handler(SIGQUIT, &handler) < 0) perror("SIGQUIT\n");
    if(install_handler(SIGTSTP, &handler) < 0) perror("SIGTSTP\n");

    int i;
    

    for(i = 0 ; i < 30; i++){
        if(i!= SIGINT && i != SIGQUIT && i != SIGTSTP )
            sigaddset (&block_mask, i);
    }
    while(1) {
        if(!read_and_echo()) break;
        if(knocked == 3) break;
    }
    return 0;
}

int read_and_echo(){
    char buffer[100];
    int returnValue = read(0, buffer, sizeof(buffer));
    memset(buffer, 0 , sizeof(buffer));
    if(!returnValue) {
        return returnValue;
    }
    write(1, buffer, strlen(buffer));
    return 1;
}

void handler(int signum){
    
    if(caughtSignal == 3) {
        caughtSignal = 0;
        t_s = -1;
        knocked = 0;
        prev_sig = -1;
    }

    printf("%d %d %ld %ld %d %d\n", signum , knocked, t_s, t_e, prev_sig, caughtSignal);
    
    if(t_s == -1)
        time(&t_s);
    
    time(&t_e);
        
    if(t_e - t_s < 2){
        if(signum == SIGINT && prev_sig == -1) knocked++;
        else if(signum == SIGTSTP && prev_sig == SIGINT) knocked++;
        else if(signum == SIGQUIT && prev_sig == SIGTSTP) knocked++;
    }else{
        t_s = -1;
        knocked = 0;
        prev_sig = -1;
    }
    if(knocked == 3) exit(0);
    prev_sig = signum;
    caughtSignal++;
}

int install_handler(int signum, void(*handler)(int)){
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_mask =  block_mask;
    return sigaction(signum, &act, NULL);
}
