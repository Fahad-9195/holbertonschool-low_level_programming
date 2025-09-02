/**
 * cap_string - capitalizes all words of a string
 * @s: the input string
 *
 * Return: pointer to the modified string
 */
char *cap_string(char *s)
{
int i = 0;
int j;
char sep[] = " \t\n,;.!?\"(){}";
while (s[i] != '\0')
{
if (i == 0 && s[i] >= 'a' && s[i] <= 'z')
s[i] -= 32;
for (j = 0; sep[j] != '\0'; j++)
{
if (s[i] == sep[j] && s[i + 1] >= 'a' && s[i + 1] <= 'z')
{
s[i + 1] -= 32;
break;
}
}
i++;
}
return (s);
}

