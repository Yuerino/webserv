//
// env.c
//

#include <stdio.h>
#include <unistd.h>

int	main(int argc, char *argv[], char *envp[])
{
	char buffer[1025];
	int read_size;

	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
	while (*envp != NULL)
	{
		printf("%s\n", *envp);
		envp++;
	}
	printf("\n");
	read_size = read(STDIN_FILENO, buffer, 1024);
	if (read_size == 0)
		printf("ERROR!!!!!\n");
	else
		printf("%s\n", buffer);
	return 0;
}
