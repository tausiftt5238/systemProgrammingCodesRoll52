#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	int k = 3;
	pid_t pid = fork();
	if (pid > 0) {
		pid = fork();
		k += 10;
		if (pid > 0) {
			k += 20; wait();
		}
	}
	printf("%d\n", k);
	return 0;
}
