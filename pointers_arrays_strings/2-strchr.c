#include "main.h"
#include <stddef.h> /* for NULL */

/**
 * _strchr - locates a character in a string
 * @s: the string to search
 * @c: the character to find
 *
 * Return: pointer to first occurrence of c in s,
 *         or NULL if not found
 */
char *_strchr(char *s, char c)
{
int i = 0;
while (s[i] != '\0')
{
if (s[i] == c)
return (&s[i]);
i++;
}
if (c == '\0')
return (&s[i]);
return (NULL);
}
