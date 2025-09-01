#include "main.h"

/**
 * print_number - prints an integer using _putchar
 * @n: the integer to print
 *
 * Return: void
 */
void print_number(int n)
{
	if (n < 0)
	{
		_putchar('-');
		/* تعامُل مع السالب: حوله لموجب باستخدام -n */
		n = -n;
	}

	/* إذا عنده أكثر من رقم، نطبع الأرقام قبل الأخير */
	if (n / 10)
		print_number(n / 10);

	/* نطبع آخر رقم */
	_putchar((n % 10) + '0');
}
