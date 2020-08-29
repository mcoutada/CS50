
"""
Credit: Python version
test:  378282246310005 --> AMEX
http://www.ee.unb.ca/cgi-bin/tervo/luhn.pl?N=378282246310005
"""


def main():
    # I assume I receive clean positive numbers per exercise statement "(i.e., devoid of hyphens, as might be printed on an actual card)"
    ccnum = int(input("number: "))

    # check for len and checksum
    if len_chk(ccnum) and checksum(ccnum):
        print("{}".format(ccbrand(ccnum)))
    else:
        print("INVALID")

# check len


def len_chk(ccn):
    return len(str(ccn)) in [13, 15, 16]

# checksum


def checksum(ccn):
    sumdig = 0
    ccnstr = str(ccn)[::-1]
    for idx, digstr in enumerate(ccnstr):
        dig = int(digstr)
        # if even, sum the digit
        if idx % 2 == 0:
            sumdig += dig
        # if odd, multiply by 2 and sum its digits
        else:
            dig *= 2
            sumdig += dig//10 + dig % 10

    return sumdig % 10 == 0

# function to translate credit card brand


def ccbrand(ccn):
    # get the first two digits
    ccn = int(str(ccn)[:2])
    # choose brand according to exercise
    if ccn in [34, 37]:
        return "AMEX"
    elif ccn in [51, 52, 53, 54, 55]:
        return "MASTERCARD"
    elif (ccn // 10 == 4):
        return "VISA"
    # if no brand found, print invalid
    else:
        return "INVALID"


main()
