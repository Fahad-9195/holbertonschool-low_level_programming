#include "lists.h"

/**
 * insert_dnodeint_at_index - inserts a new node at a given position
 * @h: double pointer to the head of the list
 * @idx: index where the new node should be added (starts at 0)
 * @n: integer value to store in the new node
 *
 * Return: address of the new node, or NULL if it failed/not possible
 */
dlistint_t *insert_dnodeint_at_index(dlistint_t **h, unsigned int idx, int n)
{
	dlistint_t *new_node, *cur;
	unsigned int i = 0;

	if (h == NULL)
		return (NULL);

	if (idx == 0)
		return (add_dnodeint(h, n));

	cur = *h;
	while (cur != NULL && i < idx - 1)
	{
		cur = cur->next;
		i++;
	}
	if (cur == NULL)
		return (NULL);

	if (cur->next == NULL)
		return (add_dnodeint_end(h, n));

	new_node = malloc(sizeof(dlistint_t));
	if (new_node == NULL)
		return (NULL);

	new_node->n = n;
	new_node->prev = cur;
	new_node->next = cur->next;
	cur->next->prev = new_node;
	cur->next = new_node;

	return (new_node);
}
