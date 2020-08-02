/* caesar exercise */
//Implement a program that implements a substitution cipher

//libraries
#include <stdio.h>  // this is needed for printf
#include <cs50.h>   // this is needed for string datatype
#include <stdlib.h> // this is needed for atoi
#include <string.h> // this is needed for strlen, strcmp

int main(int argc, string argv[])
{

    if (argc != 2)
    {
        return 1;
    }

    // convert a string to integer with atoi
    // then convert it back to string and save it into key_str
    // atoi("123") = 123, atoi("12a") = 12, atoi("a123") = 0
    // so if argv[1] is all numbers, it won't change through convertions
    int key = atoi(argv[1]);
    char key_str[strlen(argv[1])];
    sprintf(key_str, "%d", key);

    // we need strcmp to compare string contents, returns 0 if the strings are equal
    if (strcmp(argv[1], key_str) != 0)
    {
        return 1;
    }

    string s = get_string("plaintext: ");

    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
        {
            s[i] = (((s[i] - 'a') + key) % 26) + 'a';
        }
        else if (s[i] >= 'A' && s[i] <= 'Z')
        {
            s[i] = (((s[i] - 'A') + key) % 26) + 'A';
        }
    }
    printf("ciphertext: %s\n", s);
}