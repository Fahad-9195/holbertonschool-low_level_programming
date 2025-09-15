#include <stdlib.h>

/**
 * str_concat - concatenates two strings into a newly allocated space
 * @s1: first string
 * @s2: second string
 *
 * Return: pointer to the new string, or NULL if it fails
 */
char *str_concat(char *s1, char *s2)
{
    char *concat;
    unsigned int len1 = 0, len2 = 0, i, j;

    /* Treat NULL as empty string */
    if (s1 == NULL)
        s1 = "";
    if (s2 == NULL)
        s2 = "";

    /* Get lengths of s1 and s2 */
    while (s1[len1] != '\0')
        len1++;
    while (s2[len2] != '\0')
        len2++;

    /* Allocate memory for concatenated string + null terminator */
    concat = malloc((len1 + len2 + 1) * sizeof(char));
    if (concat == NULL)
        return (NULL);

    /* Copy s1 into concat */
    for (i = 0; i < len1; i++)
        concat[i] = s1[i];

    /* Copy s2 into concat after s1 */
    for (j = 0; j < len2; j++)
        concat[i + j] = s2[j];

    /* Add null terminator */
    concat[i + j] = '\0';

    return (concat);
}
