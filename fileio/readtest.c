#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(){
	int fd;
	fd = open("hello", O_RDONLY);
	if(fd == -1){
		perror("open\n");
	}
	char *buf = malloc(100);
	int ret = read(fd, buf, 5);
	if(ret == -1) perror("read");
	printf("%d\n",ret);
	 
	return 0;
}
