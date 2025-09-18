#include <stdlib.h>
#include "dog.h"

/**
 * new_dog - creates a new dog
 * @name: dog's name
 * @age: dog's age
 * @owner: dog's owner
 *
 * Return: pointer to new dog_t; NULL if function fails
 */
dog_t *new_dog(char *name, float age, char *owner)
{
	dog_t *ndog;
	char *nname, *nowner;
	int i, len_name = 0, len_owner = 0;

	if (name == NULL || owner == NULL)
		return (NULL);

	while (name[len_name] != '\0')
		len_name++;
	while (owner[len_owner] != '\0')
		len_owner++;

	ndog = malloc(sizeof(dog_t));
	if (ndog == NULL)
		return (NULL);

	nname = malloc((len_name + 1) * sizeof(char));
	if (nname == NULL)
	{
		free(ndog);
		return (NULL);
	}
	for (i = 0; i < len_name; i++)
		nname[i] = name[i];
	nname[i] = '\0';

	nowner = malloc((len_owner + 1) * sizeof(char));
	if (nowner == NULL)
	{
		free(nname);
		free(ndog);
		return (NULL);
	}
	for (i = 0; i < len_owner; i++)
		nowner[i] = owner[i];
	nowner[i] = '\0';

	ndog->name = nname;
	ndog->age = age;
	ndog->owner = nowner;

	return (ndog);
}
