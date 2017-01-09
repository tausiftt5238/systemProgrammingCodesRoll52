#include <signal.h>
#include <stdio.h>

struct two_int { int a, b ;} data;

void signal_handler(int signum){
	printf("%d %d\n",data.a, data.b);
	alarm(1);
}

int main(void){
	static struct two_int zeros = {0,0} , ones = {1,1};
	signal(SIGALRM, signal_handler);
	data = zeros;
	alarm(1);

	while(1){
		data.a = 0; data.b = 0;
		data.a = 1 ; data.b = 1 ;
	}
	return 0;
}
