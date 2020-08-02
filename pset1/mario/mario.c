#include <stdio.h>
#include <cs50.h>

void spaces(int top);
void hashes(int top);

int main()
{
// get vaild number

    int n;
    do
    {
        n = get_int("Height: ");
    }
    while (n < 1 || n > 8);

// Loop to print rows
    for (int row = 1; row <= n; row++)
    {
        spaces(n - row);
        hashes(row);
        printf("  ");
        hashes(row);
        printf("\n");
    }
}

void spaces(int top)
{
// Loop to print trailing spaces in a row
    for (int c = 1; c <= top; c++)
    {
        printf(" ");
    }
}

void hashes(int top)
{
// Loop to print hashes in a row
    for (int c = 1; c <= top; c++)
    {
        printf("#");
    }
}