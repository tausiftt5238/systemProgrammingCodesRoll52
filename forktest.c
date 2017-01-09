#include <unistd.h>
#include <stdio.h>

int main(){
	int answer = 84;
	printf("Answer : %d by %d", answer, getpid());
	fork();
	return 0;
}
