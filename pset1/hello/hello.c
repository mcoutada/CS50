#include <stdio.h>
#include <cs50.h>

int main(void)
{
    //ask name
    string name = get_string("What is your name?\n");

    //print hello
    printf("hello, %s\n", name);
}