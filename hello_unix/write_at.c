#include "apue.h"
#include <fcntl.h>

int main(int argc, char * argv[])
{
	
	if(argc < 3)
		printf("Usage: %s filename offset char cnt\n", argv[0]), exit(0);

	int fd = open(argv[1], O_WRONLY | O_CREAT, FILE_MODE);
	
	if(fd == -1)
		perror("open error"), exit(-1);

	int offset = atoi(argv[2]);
	int ch = argv[3][0];
	int cnt = atoi(argv[4]);

	if(lseek(fd, offset, SEEK_SET) == -1)
		perror("lseek error"), exit(-1);

	char * buf = (char*)malloc(cnt);
	if(buf == NULL)
		perror("malloc failed"), exit(-1);

	memset(buf, ch, cnt);
		
	if(write(fd, buf, cnt) == -1)
		perror("write error"),  exit(-1);

	free(buf);
	close(fd);

	exit(0);
}
