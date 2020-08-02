/* Substitution exercise */
//Implement a program that implements a substitution cipher

// libraries
#include <stdio.h>  // this is needed for printf
#include <cs50.h>   // this is needed for string datatype
#include <string.h> // this is needed for strlen
#include <ctype.h>  // this is needed for isupper, islower, toupper, tolower

// functions declarations
bool invalid(int argcnt, string keystr);
void lowercase(string str);

int main(int argc, string argv[])
{
    // if not valid return with 1 from main
    if (invalid(argc, argv[1]))
    {
        return 1;
    }

    string str = get_string("plaintext: ");

    // lowercase all keys
    lowercase(argv[1]);

    // cypher text
    for (int i = 0, idx; i < strlen(str); i++)
    {
        idx = tolower(str[i]) - 'a';

        if (islower(str[i]))
        {
            str[i] = argv[1][idx];
        }
        else if (isupper(str[i]))
        {
            str[i] = toupper(argv[1][idx]);
        }
    }
    printf("ciphertext: %s\n", str);
}

void lowercase(string str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (isupper(str[i]))
        {
            str[i] = tolower(str[i]);
        }
    }
}

bool invalid(int argcnt, string keystr)
{

    if (argcnt != 2)
    {
        return true;
    }

    int n = strlen(keystr);

    if (n != 26)
    {
        return true;
    }

    for (int i = 0; i < n; i++)
    {
        if (!((keystr[i] >= 'a' && keystr[i] <= 'z') || (keystr[i] >= 'A' && keystr[i] <= 'Z')))
        {
            return true;
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {

            if (keystr[i] == keystr[j])
            {
                return true;
            }
        }
    }

    return false;
}