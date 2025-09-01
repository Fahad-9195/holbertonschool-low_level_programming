#include <stdio.h>

/**
 * main - prints the largest prime factor of 612852475143
 *
 * Return: Always 0
 */
int main(void)
{
	long long n = 612852475143LL;
	long long largest = 1;
	long long f = 2;

	/* اقسم على 2 قدر الإمكان */
	while (n % f == 0)
	{
		largest = f;
		n /= f;
	}

	/* جرّب العوامل الفردية من 3 وصاعداً */
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

	/* إن تبقى n > 1 فهو عامل أولي أكبر من الجذر */
	if (n > 1)
		largest = n;

	printf("%lld\n", largest);
	return (0);
}
