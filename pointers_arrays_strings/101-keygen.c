#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * main - generates a random valid password for the program 101-crackme
 *
 * Return: Always 0.
 */
int main(void)
{
	char pass[84];
	int i = 0, sum = 0, c;

	srand((unsigned int)time(NULL));

	/* keep sum in [2646..2739] so the last char stays printable (33..126) */
	while (sum < 2772 - 126)
	{
		c = (rand() % 94) + 33;          /* printable ASCII: 33..126 */
		if (sum + c > 2772 - 33)         /* avoid exceeding 2739 */
			continue;
		pass[i++] = (char)c;
		sum += c;
	}

	/* final char makes total sum exactly 2772 */
	pass[i++] = (char)(2772 - sum);
	pass[i] = '\0';

	printf("%s", pass);
	return (0);
}
