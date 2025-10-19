#include "hash_tables.h"

/**
 * node_create - Creates a new hash node (key/value duplicated)
 * @key: Key string (non-empty)
 * @value: Value string
 *
 * Return: Pointer to new node, or NULL on failure
 */
static hash_node_t *node_create(const char *key, const char *value)
{
	hash_node_t *node;

	node = malloc(sizeof(hash_node_t));
	if (node == NULL)
		return (NULL);

	node->key = strdup(key);
	if (node->key == NULL)
	{
		free(node);
		return (NULL);
	}

	node->value = strdup(value);
	if (node->value == NULL)
	{
		free(node->key);
		free(node);
		return (NULL);
	}

	node->next = NULL;
	return (node);
}

/**
 * update_if_exists - Updates node value if key exists in a chain
 * @head: Head of chain
 * @key: Key to search
 * @value: New value (will be duplicated)
 *
 * Return: 1 if updated, 0 if not found, -1 on failure
 */
static int update_if_exists(hash_node_t *head, const char *key,
			    const char *value)
{
	char *vdup;

	while (head != NULL)
	{
		if (strcmp(head->key, key) == 0)
		{
			vdup = strdup(value);
			if (vdup == NULL)
				return (-1);
			free(head->value);
			head->value = vdup;
			return (1);
		}
		head = head->next;
	}
	return (0);
}

/**
 * hash_table_set - Adds or updates an element in a hash table
 * @ht: Pointer to the hash table
 * @key: The key (cannot be an empty string)
 * @value: The value associated with the key (will be duplicated)
 *
 * Return: 1 if it succeeded, 0 otherwise
 */
int hash_table_set(hash_table_t *ht, const char *key, const char *value)
{
	unsigned long int index;
	hash_node_t *new_node;
	int upd;

	if (ht == NULL || key == NULL || *key == '\0' || value == NULL)
		return (0);

	index = key_index((const unsigned char *)key, ht->size);

	/* try update in-place if key exists */
	upd = update_if_exists(ht->array[index], key, value);
	if (upd == 1)
		return (1);
	if (upd == -1)
		return (0);

	/* insert new node at head (chaining) */
	new_node = node_create(key, value);
	if (new_node == NULL)
		return (0);

	new_node->next = ht->array[index];
	ht->array[index] = new_node;

	return (1);
}

