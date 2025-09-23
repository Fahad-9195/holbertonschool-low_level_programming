#include <stdarg.h>
#include <stdio.h>

/**
 * print_numbers - prints numbers separated by a string, followed by a new line
 * @separator: string printed between numbers (NULL = no separator)
 * @n: number of integers passed to the function
 */
void print_numbers(const char *separator, const unsigned int n, ...)
{
	va_list ap;
	unsigned int i;

	va_start(ap, n);

	for (i = 0; i < n; i++)
	{
		printf("%d", va_arg(ap, int));

		if (separator != NULL && i < n - 1)
			printf("%s", separator);
	}

	va_end(ap);

	printf("\n");
}
