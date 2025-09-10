#include <stdio.h>
#include <stdlib.h>

/**
 * main - multiplies two numbers and prints the result
 * @argc: argument count
 * @argv: argument vector (expects two numbers at argv[1], argv[2])
 *
 * Return: 0 on success, 1 if wrong number of arguments.
 */
int main(int argc, char *argv[])
{
int a, b, product;
if (argc != 3)
{
printf("Error\n");
return (1);
}
a = atoi(argv[1]);
b = atoi(argv[2]);
product = a * b;
printf("%d\n", product);
return (0);
}
