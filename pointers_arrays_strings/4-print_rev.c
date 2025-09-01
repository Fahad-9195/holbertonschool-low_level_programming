#include "main.h"

/**
 * print_rev - prints a string in reverse, followed by a new line
 * @s: pointer to the string
 *
 * Return: nothing
 */
void print_rev(char *s)
{
	int length = 0;

	/* Count string length */
	while (s[length] != '\0')
		length++;

	/* Print in reverse */
	for (length = length - 1; length >= 0; length--)
		_putchar(s[length]);

	_putchar('\n');
}

