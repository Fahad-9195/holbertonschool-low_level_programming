#include "main.h"

/**
 * _strlen_recursion - returns length of string
 * @s: string
 *
 * Return: length
 */
int _strlen_recursion(char *s)
{
if (*s == '\0')
return (0);
return (1 + _strlen_recursion(s + 1));
}

/**
 * check_pal - helper function to check palindrome
 * @s: string
 * @start: starting index
 * @end: ending index
 *
 * Return: 1 if palindrome, 0 otherwise
 */
int check_pal(char *s, int start, int end)
{
if (start >= end)
return (1);
if (s[start] != s[end])
return (0);
return (check_pal(s, start + 1, end - 1));
}

/**
 * is_palindrome - checks if string is palindrome
 * @s: string to check
 *
 * Return: 1 if palindrome, 0 otherwise
 */
int is_palindrome(char *s)
{
int len = _strlen_recursion(s);
if (len == 0)
return (1);
return (check_pal(s, 0, len - 1));
}
