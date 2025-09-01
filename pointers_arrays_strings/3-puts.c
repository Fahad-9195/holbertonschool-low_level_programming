#include "main.h"
/**
 * _puts - prints a string, followed by a new line, to stdout
 * @str: pointer to the string
 *
 * This function prints the characters of a string one by one
 * using the _putchar function, then prints a newline.
 *
 * Return: nothing (void)
 */
void _puts(char *str)
{
int i = 0;
while (str[i] != '\0')
{
_putchar(str[i]);
i++;
}
_putchar('\n');
}
