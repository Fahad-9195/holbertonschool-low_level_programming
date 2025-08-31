#include "main.h"

/**
 * print_diagonal - draws a diagonal line on the terminal
 * @n: number of times the character \ should be printed
 *
 * Return: void
 */
void print_diagonal(int n)
{
int row, space;
if ( n <= 0)
{
_putchar('\n');
}
else
{
for (row = 0; row < n; row++)
{
for (space = 0; space < n; space++)
_putchar(' ');
_putchar('\\');
_putchar('\n');
}
}
}

