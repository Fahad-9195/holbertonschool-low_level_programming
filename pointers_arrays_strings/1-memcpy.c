#include "main.h"

/**
 * _memcpy - copies memory area
 * @dest: destination memory area (where to copy)
 * @src: source memory area (from where to copy)
 * @n: number of bytes to copy
 *
 * Return: pointer to dest
 */
char *_memcpy(char *dest, char *src, unsigned int n)
{
unsigned int i;
for (i = 0; i < n; i++) /* ننسخ n بايت واحد واحد */
{
dest[i] = src[i];
}
return (dest);
}
