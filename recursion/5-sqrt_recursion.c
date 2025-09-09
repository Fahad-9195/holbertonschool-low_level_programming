#include "main.h"

/**
 * sqrt_helper - helper function to find sqrt recursively
 * @n: number
 * @guess: current guess
 *
 * Return: sqrt of n or -1
 */
int sqrt_helper(int n, int guess)
{
if (guess * guess == n)
return (guess);
if (guess * guess > n)
return (-1);
return (sqrt_helper(n, guess + 1));
}

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

