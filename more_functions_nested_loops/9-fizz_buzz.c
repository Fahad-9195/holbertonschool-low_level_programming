#include "main.h"

/**
 * main - prints numbers from 1 to 100 with FizzBuzz logic
 *
 * Return: Always 0
 */
int main(void)
{
    int i;

    for (i = 1; i <= 100; i++)
    {
        if (i % 3 == 0 && i % 5 == 0)
        {
            _putchar('F'); _putchar('i'); _putchar('z'); _putchar('z');
            _putchar('B'); _putchar('u'); _putchar('z'); _putchar('z');
        }
        else if (i % 3 == 0)
        {
            _putchar('F'); _putchar('i'); _putchar('z'); _putchar('z');
        }
        else if (i % 5 == 0)
        {
            _putchar('B'); _putchar('u'); _putchar('z'); _putchar('z');
        }
        else
        {
            print_number(i);
        }

        if (i < 100)
            _putchar(' ');
    }
    _putchar('\n');

    return (0);
}

