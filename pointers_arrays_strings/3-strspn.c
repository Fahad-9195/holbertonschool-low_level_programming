#include "main.h"

/**
 * _strspn - gets the length of a prefix substring
 * @s: the string to search
 * @accept: the accepted characters
 *
 * Return: number of bytes in the initial segment of s
 *         consisting only of bytes from accept
 */
unsigned int _strspn(char *s, char *accept)
{
unsigned int i, j;
int match;
for (i = 0; s[i] != '\0'; i++)
{
match = 0;
for (j = 0; accept[j] != '\0'; j++)
{
if (s[i] == accept[j])
{
match = 1;
break;
}
}
if (!match)
return (i);
}
return (i);
}
