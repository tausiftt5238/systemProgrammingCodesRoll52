#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define DEVICE "/dev/sysprogdevice"

int main(){
	char ch;
	char read_buf[100], write_buf[100];

	int fd = open(DEVICE, O_RDWR);
	if(fd == -1){
		printf("file %s either does not exist or has been locked by others",DEVICE);
		exit(-1);
	}

	printf("r = read from device, w= write to device, enter command: ");
	scanf("%c" , &ch);
	getchar();
	switch (ch)
	{
		case 'w':
		printf("enter data: ");
		scanf("%[^\n]", write_buf);
		write(fd, write_buf, strlen(write_buf));
		break;
		case 'r':
		printf("Enter amount: ");
		int count = 0;
		scanf("%d", &count);
		read(fd, read_buf, count);
		printf("device: %s\n", read_buf);
		break;

		default:
		printf("command not recognized\n");
		break;
	}
	return 0;
}
