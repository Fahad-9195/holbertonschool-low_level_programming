#include <stdlib.h>
#include "main.h"

/**
 * string_nconcat - concatenates two strings
 * @s1: first string
 * @s2: second string
 * @n: number of bytes from s2 to concatenate
 *
 * Return: pointer to newly allocated space containing s1
 *         followed by first n bytes of s2, null-terminated.
 *         If function fails, return NULL.
 *         If s1 or s2 is NULL, treat as empty string.
 */
char *string_nconcat(char *s1, char *s2, unsigned int n)
{
    char *ptr;
    unsigned int i, j, len1 = 0, len2 = 0;

    if (s1 == NULL)
        s1 = "";
    if (s2 == NULL)
        s2 = "";

    /* Calculate lengths */
    while (s1[len1] != '\0')
        len1++;
    while (s2[len2] != '\0')
        len2++;

    if (n >= len2)
        n = len2;

    /* allocate memory for s1 + n chars from s2 + '\0' */
    ptr = malloc((len1 + n + 1) * sizeof(char));
    if (ptr == NULL)
        return (NULL);

    /* copy s1 */
    for (i = 0; i < len1; i++)
        ptr[i] = s1[i];

    /* copy n bytes from s2 */
    for (j = 0; j < n; j++)
        ptr[i + j] = s2[j];

    ptr[i + j] = '\0';

   return (ptr);
}
