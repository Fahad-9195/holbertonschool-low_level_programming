#include <stdio.h>
#include "lists.h"

size_t print_list(const list_t *h)
{
    size_t count = 0; // عدد العقد

    // طالما h ليست NULL (يعني لسه فيه عناصر)
    while (h)
    {
        // لو النص فاضي (NULL)
        if (h->str == NULL)
            printf("[0] (nil)\n");
        else
            printf("[%u] %s\n", h->len, h->str);

        // انتقل للعقدة التالية
        h = h->next;
        count++; // زِد العداد
    }

    return (count); // رجّع عدد العقد
}
