#include <stdio.h>
#include <stdlib.h>

/**
 * main - prints the opcodes of its own main function
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success
 */
int main(int argc, char *argv[])
{
	int n, i;
	unsigned char *p;

	if (argc != 2)
	{
		printf("Error\n");
		exit(1);
	}

	n = atoi(argv[1]);
	if (n < 0)
	{
		printf("Error\n");
		exit(2);
	}

	p = (unsigned char *)(void *)main;

	for (i = 0; i < n; i++)
	{
		printf("%02x", p[i]);
		if (i < n - 1)
			printf(" ");
		else
			printf("\n");
	}

	return (0);
}
