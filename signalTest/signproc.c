#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void print_sigset(sigset_t *set){
	int i;
	for(i = 1 ; i < NSIG ; i++){
		if(sigismember(set , i))
			printf("%s\n", strsignal(i));
	}
}

void main(){
	sigset_t set, oset, pset;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigprocmask( SIG_BLOCK, &set, &oset);
	printf("Old set was \n"); print_sigset(&pset);
	sigpending(&pset);
	printf("Pending set is \n"); print_sigset( &pset);
	kill(getpid(), SIGINT);
	sigpending( &pset);
	printf("Pending set is \n"); print_sigset( &oset);
	sigprocmask(SIG_UNBLOCK, &set, &oset);
}
