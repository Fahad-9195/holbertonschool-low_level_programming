#include "main.h"

/**
 * print_last_digit - prints the last digit of a number
 * @n: The number from which we extract the last digit
 *
 * Description: This function computes the last digit of a number,
 * prints it using _putchar, and returns its value.
 *
 * Return: The value of the last digit
 */
int print_last_digit(int n)
{
	int last;

	last = n % 10;

	if (last < 0)
		last = -last;

	_putchar(last + '0');
	return (last);
}

