#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
	struct stat buf1;
	struct stat buf2;
	stat("hello", &buf1);
	lstat("hello", &buf2);
	printf("%d %d %d %d\n", buf1.st_ino, buf1.st_nlink, buf1.st_uid, buf1.st_size);
	
	printf("%d %d %d %d\n", buf2.st_ino, buf2.st_nlink, buf2.st_uid, buf2.st_size);
	return 0;
}
