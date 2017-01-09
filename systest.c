#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
	printf("%d\n",system("ls -la"));
	return 0;
}
