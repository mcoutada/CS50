/* readability exercise */
//Design and implement a program, readability, that computes the Coleman-Liau index of the text.

//libraries
#include <stdio.h>  // this is needed for printf
#include <cs50.h>   // this is needed for get_string
#include <string.h> // this is needed for strlen
#include <math.h>   // this is needed for round
#include <ctype.h>  // this is needed for isalpha

int main(void)
{
    string str = get_string("Text: ");
    int letters = 0, words = 0, sentences = 0;

    for (int i = 0; i < strlen(str); i++)
    {

        if (isalpha(str[i]))
        {
            letters++;
        }
        else if (str[i] == ' ' || str[i + 1] == '\0')
        {
            words++;
        }

        if (str[i] == '?' || str[i] == '!' || str[i] == '.')
        {
            sentences++;
        }

        // printf("%c - letters: %i; words: %i; sentences: %i\n",str[i], letters, words, sentences);
    }
    // Turn it into floating point division by making at least one operand a floating point number
    float letters_avg = 100 * (float)letters / words;
    float sentences_avg = 100 * (float)sentences / words;
    int cl_idx = round(0.0588 * letters_avg - 0.296 * sentences_avg - 15.8);

    // printf("letters_avg: %f; sentences_avg: %f; cl_idx: %i\n",letters_avg, sentences_avg, cl_idx);

    if (cl_idx < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (cl_idx >= 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %i\n", cl_idx);
    }
}