#include "main.h"

/**
 * wildcmp - compares two strings, considering * as wildcard
 * @s1: first string
 * @s2: second string (may contain *)
 *
 * Return: 1 if considered identical, 0 otherwise
 */
int wildcmp(char *s1, char *s2)
{
if (*s1 == '\0' && *s2 == '\0')
return (1);
if (*s2 == '*')
{
/* * can match empty string or move s1 forward */
if (wildcmp(s1, s2 + 1))
return (1);
if (*s1 != '\0' && wildcmp(s1 + 1, s2))
return (1);
return (0);
}
if (*s1 == *s2 && *s1 != '\0')
return (wildcmp(s1 + 1, s2 + 1));
return (0);
}
