/* credit card exercise */
//credit card exercise

//libraries
#include <stdio.h> // this is needed for printf
#include <cs50.h> // this is needed for get_long

//function declaration
bool len_chk(long ccn);
bool checksum(long ccn);
char *ccbrand(long ccn);

int main()
{

    // I assume I receive clean positive numbers per exercise statement "(i.e., devoid of hyphens, as might be printed on an actual card)""
    long ccnum = get_long("number: ");

    // check for len and checksum
    if (len_chk(ccnum) && checksum(ccnum))
    {
        printf("%s\n", ccbrand(ccnum));
    }
    else
    {
        printf("INVALID\n");
    }
}

// check len
bool len_chk(long ccn)
{
    int len = 0;
    while (ccn != 0)
    {
        len++;
        ccn/=10;
    }

    return len == 13 || len == 15 || len == 16;
}

// checksum
bool checksum(long ccn)
{
    int digit, sum = 0;

//loop every digit
    int i = 0;
    while (ccn != 0)
    {

        //save the digit from the top right
        digit = ccn % 10;

        //if even, sum the digit
        if (i % 2 == 0)
        {
            sum += digit;
        }
        //if odd, multiply by 2 and sum its digits
        else
        {
            digit *= 2;
            sum += digit/10 + digit % 10;
        }

        i++;
        ccn /= 10;
    }
    //return true if number is 10, 20 , 30, etc
    return sum % 10 == 0;
}

// function to translate credit card brand
char *ccbrand(long ccn)
{
    //get the first two digits
    while (ccn >= 100)
    {
        ccn /= 10;
    }

    //per readability, I use a variable, then return at the end
    string ccbrand;

    // choose brand according to exercise
    if (ccn == 34 || ccn == 37)
    {
        ccbrand = "AMEX";
    }
    else if (ccn == 51 || ccn == 52 || ccn == 53 || ccn == 54 || ccn == 55)
    {
        ccbrand = "MASTERCARD";
    }
    else if (ccn / 10 == 4)
    {
        ccbrand = "VISA";
    }
    // if no brand found, print invalid
    else
    {
        ccbrand = "INVALID";
    }

    //return whatever found
    return ccbrand;
}