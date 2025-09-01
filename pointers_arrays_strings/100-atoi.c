#include "main.h"

/**
 * _atoi - converts a string to an integer
 * @s: pointer to the string
 *
 * Return: the integer value, or 0 if no numbers are found
 */
int _atoi(char *s)
{
	int i = 0;
	int sign = 1;
	int result = 0;
	int found_digit = 0;

	/* حساب الإشارة */
	while (s[i] != '\0' && !(s[i] >= '0' && s[i] <= '9'))
	{
		if (s[i] == '-')
			sign *= -1;
		else if (s[i] == '+')
			;
		i++;
	}

	/* بناء الرقم مع الإشارة من البداية */
	while (s[i] >= '0' && s[i] <= '9')
	{
		found_digit = 1;

		if (sign > 0)
			result = result * 10 + (s[i] - '0');
		else
			result = result * 10 - (s[i] - '0');

		i++;
	}

	if (!found_digit)
		return (0);

	return (result);
}

