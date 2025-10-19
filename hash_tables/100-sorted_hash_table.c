#include "hash_tables.h"

/**
 * shash_node_create - Creates a new sorted hash node
 * @key: Key string (must be non-empty)
 * @value: Value string (will be duplicated)
 *
 * Return: Pointer to new node or NULL on failure
 */
static shash_node_t *shash_node_create(const char *key, const char *value)
{
	shash_node_t *node;

	node = malloc(sizeof(shash_node_t));
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
	node->sprev = NULL;
	node->snext = NULL;
	return (node);
}

/**
 * shash_table_create - Creates a sorted hash table
 * @size: Size of the array
 *
 * Return: Pointer to the new table, or NULL on failure
 */
shash_table_t *shash_table_create(unsigned long int size)
{
	shash_table_t *ht;
	unsigned long int i;

	if (size == 0)
		return (NULL);

	ht = malloc(sizeof(shash_table_t));
	if (ht == NULL)
		return (NULL);

	ht->size = size;
	ht->array = malloc(sizeof(shash_node_t *) * size);
	if (ht->array == NULL)
	{
		free(ht);
		return (NULL);
	}
	for (i = 0; i < size; i++)
		ht->array[i] = NULL;

	ht->shead = NULL;
	ht->stail = NULL;
	return (ht);
}

/**
 * sorted_insert - Inserts a node into the sorted dll by ASCII key
 * @ht: Sorted hash table
 * @node: Node to insert (assumes node->sprev/snext are NULL)
 *
 * Return: void
 */
static void sorted_insert(shash_table_t *ht, shash_node_t *node)
{
	shash_node_t *cur;

	if (ht->shead == NULL)
	{
		ht->shead = node;
		ht->stail = node;
		return;
	}

	cur = ht->shead;
	while (cur && strcmp(node->key, cur->key) > 0)
		cur = cur->snext;

	if (cur == ht->shead)
	{
		/* insert at head */
		node->snext = ht->shead;
		ht->shead->sprev = node;
		ht->shead = node;
	}
	else if (cur == NULL)
	{
		/* insert at tail */
		node->sprev = ht->stail;
		ht->stail->snext = node;
		ht->stail = node;
	}
	else
	{
		/* insert before cur */
		node->snext = cur;
		node->sprev = cur->sprev;
		cur->sprev->snext = node;
		cur->sprev = node;
	}
}

/**
 * chain_find - Finds a key in a bucket chain
 * @head: Head of a bucket chain
 * @key: Key to search
 *
 * Return: Pointer to node if found, NULL otherwise
 */
static shash_node_t *chain_find(shash_node_t *head, const char *key)
{
	while (head)
	{
		if (strcmp(head->key, key) == 0)
			return (head);
		head = head->next;
	}
	return (NULL);
}

/**
 * shash_table_set - Adds/updates an element in a sorted hash table
 * @ht: Sorted hash table
 * @key: Key (cannot be an empty string)
 * @value: Value (will be duplicated, can be empty string)
 *
 * Return: 1 on success, 0 on failure
 */
int shash_table_set(shash_table_t *ht, const char *key, const char *value)
{
	unsigned long int idx;
	shash_node_t *n, *exist;
	char *vdup;

	if (ht == NULL || key == NULL || *key == '\0' || value == NULL)
		return (0);

	idx = key_index((const unsigned char *)key, ht->size);
	exist = chain_find(ht->array[idx], key);
	if (exist)
	{
		vdup = strdup(value);
		if (vdup == NULL)
			return (0);
		free(exist->value);
		exist->value = vdup;
		return (1);
	}

	n = shash_node_create(key, value);
	if (n == NULL)
		return (0);

	/* insert at head of bucket for chaining */
	n->next = ht->array[idx];
	ht->array[idx] = n;

	/* insert into sorted doubly linked list */
	sorted_insert(ht, n);

	return (1);
}

/**
 * shash_table_get - Retrieves the value for a key
 * @ht: Sorted hash table
 * @key: Key to look for
 *
 * Return: Value pointer, or NULL if not found
 */
char *shash_table_get(const shash_table_t *ht, const char *key)
{
	unsigned long int idx;
	shash_node_t *n;

	if (ht == NULL || key == NULL || *key == '\0')
		return (NULL);

	idx = key_index((const unsigned char *)key, ht->size);
	n = ht->array[idx];

	while (n)
	{
		if (strcmp(n->key, key) == 0)
			return (n->value);
		n = n->next;
	}
	return (NULL);
}

/**
 * shash_table_print - Prints the table in sorted (ascending) key order
 * @ht: Sorted hash table
 *
 * Return: void
 */
void shash_table_print(const shash_table_t *ht)
{
	shash_node_t *n;
	int first = 1;

	if (ht == NULL)
		return;

	printf("{");
	n = ht->shead;
	while (n)
	{
		if (!first)
			printf(", ");
		printf("'%s': '%s'", n->key, n->value);
		first = 0;
		n = n->snext;
	}
	printf("}\n");
}

/**
 * shash_table_print_rev - Prints the table in reverse sorted order
 * @ht: Sorted hash table
 *
 * Return: void
 */
void shash_table_print_rev(const shash_table_t *ht)
{
	shash_node_t *n;
	int first = 1;

	if (ht == NULL)
		return;

	printf("{");
	n = ht->stail;
	while (n)
	{
		if (!first)
			printf(", ");
		printf("'%s': '%s'", n->key, n->value);
		first = 0;
		n = n->sprev;
	}
	printf("}\n");
}

/**
 * shash_table_delete - Deletes a sorted hash table
 * @ht: Sorted hash table
 *
 * Return: void
 */
void shash_table_delete(shash_table_t *ht)
{
	shash_node_t *n, *tmp;

	if (ht == NULL)
		return;

	n = ht->shead;
	while (n)
	{
		tmp = n->snext;
		free(n->key);
		free(n->value);
		free(n);
		n = tmp;
	}

	free(ht->array);
	free(ht);
}
