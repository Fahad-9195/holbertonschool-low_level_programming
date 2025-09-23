#include <stdarg.h>
#include <stdio.h>

/**
 * print_all - prints anything based on a format string
 * @format: list of types of args (c,i,f,s)
 */
void print_all(const char * const format, ...)
{
	va_list ap;
	unsigned int i = 0;
	char *sep = "";
	char *s;

	va_start(ap, format);

	while (format && format[i])
	{
		switch (format[i])
		{
		case 'c':
			printf("%s%c", sep, va_arg(ap, int));
			sep = ", ";
			break;
		case 'i':
			printf("%s%d", sep, va_arg(ap, int));
			sep = ", ";
			break;
		case 'f':
			printf("%s%f", sep, va_arg(ap, double));
			sep = ", ";
			break;
		case 's':
			s = va_arg(ap, char *);
			if (s == NULL) /* 1st and only needed if */
				s = "(nil)";
			printf("%s%s", sep, s);
			sep = ", ";
			break;
		default:
			break;
		}
		i++;
	}

	va_end(ap);
	printf("\n");
}
