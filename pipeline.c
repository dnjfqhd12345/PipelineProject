#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>

#define MAXLINE 100
#define READ 0
#define WRITE 1

int main(int argc, char* argv[])
{
	char str[1024] = "";
	char* cmd1;
	char* cmd1temp[20];
	char* cmd2;
	char* cmd3;
	char* cmd1arr[100] = { {""} };
	char* cmd2arr[100] = { {""} };
	char* cmd3arr[100] = { {""} };

	for (int i = 2; i < argc; i++)
	{
		strcat(str, argv[i]);
		strcat(str, " ");
	}

	cmd1 = strtok(str, ",");
	strcpy(cmd1temp, cmd1);
	cmd2 = strtok(NULL, ",");
	cmd3 = strtok(NULL, ",");
	char* ptr = strtok(cmd1, " ");
	int count = 0;
	while (ptr != NULL)
	{
		cmd1arr[count++] = ptr;
		ptr = strtok(NULL, ",");
	}
	ptr = strtok(cmd2, " ");
	count = 0;
	while (ptr != NULL)
	{
		cmd2arr[count++] = ptr;
		ptr = strtok(NULL, " ");
	}

	ptr = strtok(cmd3, " ");
	count = 0;
	while (ptr != NULL)
	{
		cmd3arr[count++] = ptr;
		ptr = strtok(NULL, " ");
	}

	char line[MAXLINE];
	int fd[2];
	pipe(fd);
	int file;
	int pid;

	FILE* fp = NULL;
	int state;
	int fdd = open(argv[1], O_CREAT | O_RDWR | O_APPEND | O_TRUNC, 0666);
	printf("\n");
	if (!strcmp(cmd1arr[0], "cd")) {
		printf("cd: builtin command\n");
	}
	else if (!strcmp(cmd1arr[0], "pwd")) {
		printf("pwd: builtin command\n");
	}
	else {
		char  buff[1024];
		fp = popen(cmd1temp, "r");
		if (NULL == fp) {
			perror("popen() 실패");
			return -1;
		}
		dup2(fdd, WRITE);
		while (fgets(buff, 1024, fp))
			printf("%s", buff);
		pclose(fp);
	}
	if ((pid = fork()) == 0) { // 자식 프로세스인 경우
		close(fd[READ]);
		dup2(fd[WRITE], 1); // 쓰기용 파이프를 표준출력에 복제
		close(fd[WRITE]);
		// 표준출력에 쓴 내용은 파이프로 전달됨
		execvp(cmd2arr[0], cmd2arr);
		perror("에러가 발생하였습니다. -자식 프로세스");
	}
	else { // 부모 프로세스인 경우
		wait(0);
		close(fd[WRITE]);
		dup2(fd[READ], 0);
		close(fd[READ]);
		int n;
		dup2(fdd, WRITE);
		execvp(cmd3arr[0], cmd3arr);
		close(fdd);

		perror("에러가 발생하였습니다. -부모 프로세스");
	}
	exit(1);
	return 0;
}
