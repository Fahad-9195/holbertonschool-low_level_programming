#include <stdarg.h>
#include <stdio.h>

/**
 * print_all - prints anything based on a format string
 * @format: list of types of arguments passed to the function
 *
 * c: char
 * i: integer
 * f: float
 * s: char * (if NULL prints (nil))
 * any other char is ignored
 */
void print_all(const char * const format, ...)
{
	va_list ap;
	int i = 0;
	char *str;
	char *sep = "";

	va_start(ap, format);

	while (format && format[i])
	{
		if (format[i] == 'c')
		{
			printf("%s%c", sep, va_arg(ap, int));
		}
		else if (format[i] == 'i')
		{
			printf("%s%d", sep, va_arg(ap, int));
		}
		else if (format[i] == 'f')
		{
			printf("%s%f", sep, va_arg(ap, double));
		}
		else if (format[i] == 's')
		{
			str = va_arg(ap, char *);
			if (str == NULL)
				str = "(nil)";
			printf("%s%s", sep, str);
		}
		if (format[i] == 'c' || format[i] == 'i' ||
		    format[i] == 'f' || format[i] == 's')
			sep = ", ";
		i++;
	}

	va_end(ap);
	printf("\n");
}
