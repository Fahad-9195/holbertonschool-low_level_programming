#include <stdio.h>

/**
 * main - prints the program name followed by a new line
 * @argc: argument count (unused)
 * @argv: argument vector; argv[0] is the invoked name (may include path)
 *
 * Return: Always 0.
 */
int main(int argc, char *argv[])
{
(void)argc;
if (argv[0] != NULL)
printf("%s\n", argv[0]);
else
printf("(unknown)\n");
return (0);
}
