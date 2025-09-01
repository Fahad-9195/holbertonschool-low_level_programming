#include <stdio.h>

/**
 * main - prints the largest prime factor of 612852475143
 *
 * Return: Always 0
 */
int main(void)
{
	unsigned long n = 612852475143UL;
	unsigned long largest = 1;
	unsigned long f;

	/* remove factors of 2 */
	while (n % 2 == 0)
	{
		largest = 2;
		n /= 2;
	}

	/* test odd factors */
	f = 3;
	while (f * f <= n)
	{
		while (n % f == 0)
		{
			largest = f;
			n /= f;
		}
		f += 2;
	}

	/* if remainder > 1, it's a prime factor (and the largest) */
	if (n > 1)
		largest = n;

	printf("%lu\n", largest);
	return (0);
}

