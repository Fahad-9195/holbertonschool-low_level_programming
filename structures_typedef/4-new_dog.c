#include <stdlib.h>
#include "dog.h"

/* local helpers (لا تُحتسب ضمن طول new_dog) */
static int _strlen_local(const char *s)
{
	int len = 0;

	if (s == NULL)
		return (0);
	while (s[len] != '\0')
		len++;
	return (len);
}

static char *_strdup_local(const char *s)
{
	char *p;
	int i, len = _strlen_local(s);

	p = malloc((len + 1) * sizeof(char));
	if (p == NULL)
		return (NULL);

	for (i = 0; i < len; i++)
		p[i] = s[i];
	p[len] = '\0';
	return (p);
}

/**
 * new_dog - creates a new dog
 * @name: dog's name (copied)
 * @age: dog's age
 * @owner: dog's owner (copied)
 *
 * Return: pointer to new dog_t, or NULL on failure
 */
dog_t *new_dog(char *name, float age, char *owner)
{
	dog_t *ndog;
	char *nname, *nowner;

	if (name == NULL || owner == NULL)
		return (NULL);

	nname = _strdup_local(name);
	if (nname == NULL)
		return (NULL);

	nowner = _strdup_local(owner);
	if (nowner == NULL)
	{
		free(nname);
		return (NULL);
	}

	ndog = malloc(sizeof(dog_t));
	if (ndog == NULL)
	{
		free(nname);
		free(nowner);
		return (NULL);
	}

	ndog->name = nname;
	ndog->age = age;
	ndog->owner = nowner;
	return (ndog);
}
