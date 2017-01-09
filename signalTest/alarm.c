#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void alarm_handler(int signum){
	printf("Time has started to move again\n");
}

int main(){
	signal(SIGALRM, alarm_handler);
	alarm(5);
	pause();
	return 0;
}
