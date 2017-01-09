#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void writer(const char* message, int count, FILE* stream){
	for( ; count >0 ; count--){
		printf("writing message here");
		fprintf(stream,"%s\n",message);
		fflush(stream);
	sleep(1);
	}
}

void reader(FILE* stream){
	char buffer[1024];
	while(!feof(stream) && !ferror(stream) && fgets(buffer, sizeof(buffer), stream) != NULL)
		fputs(buffer,stdout);
}

int main(){
	int fds[2];
	pid_t pid;
	
	pipe(fds);
	printf("%d %d\n",fds[0],fds[1]);
	pid = fork();
	if(pid == 0){
		FILE* stream;
		close(fds[1]);
		stream = fdopen(fds[0], "r");
		reader(stream);
		close(fds[0]);
	}
	else{
		FILE* stream;
		close(fds[0]);
		stream = fdopen(fds[1], "w");
		writer("Hello world", 5 , stream);
		close(fds[1]);
	}
	return 0;
}