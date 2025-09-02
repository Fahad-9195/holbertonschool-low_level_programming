/**
 * _strncat - concatenates two strings using at most n bytes from src
 * @dest: pointer to the destination string
 * @src: pointer to the source string
 * @n: maximum number of bytes to copy from src
 *
 * Return: pointer to the resulting string dest
 */
char *_strncat(char *dest, char *src, int n)
{
int d = 0;
int s = 0;
while (dest[d] != '\0')
{
d++;
}
while (s < n && src[s] != '\0')
{
dest[d] = src[s];
d++;
s++;
}
dest[d] = '\0';
return (dest);
}

