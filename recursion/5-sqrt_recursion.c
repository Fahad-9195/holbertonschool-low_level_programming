#include "main.h"
/**
 * _sqrt_recursion - returns the natural square root of a number
 * @n: number to compute sqrt of
 *
 * Return: natural sqrt of n, or -1 if none
 */
int _sqrt_recursion(int n)
{
    if (n < 0)
        return (-1);
    return (sqrt_helper(n, 0));
}

