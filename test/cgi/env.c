//
// env.c
//

#include <stdio.h>
#include <unistd.h>

int	main(int argc, char *argv[], char *envp[])
{
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
	while (*envp != NULL)
	{
		printf("%s\n", *envp);
		envp++;
	}
	return 0;
}
