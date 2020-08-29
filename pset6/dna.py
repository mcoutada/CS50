
"""
DNA
"""
from csv import DictReader
from sys import argv
from re import findall

# The arguments are:   sys.argv       ex: ['dna.py', 'databases/large.csv', 'sequences/1.txt']
# Number of arguments: len(sys.argv)  ex: 3
# Name of the script:  sys.argv[0]    ex: 'dna.py'
# CSV names file:      sys.argv[1]    ex: 'databases/large.csv'
# DNA chain TXT fie:   sys.argv[2]    ex: 'sequences/1.txt'


def main():

    if len(argv) != 3:
        print("Usage: python dna.py data.csv sequence.txt")
        return

    # extract the DNA Short Tandem Repeats (STRs) from the first line (fieldnames) of the csv
    # strKeys = ['AGATC', 'TTTTTTCT', 'AATG', 'TCTAG', 'GATA', 'TATC', 'GAAA', 'TCTG']
    with open(argv[1]) as csvFile:
        dnaDB = DictReader(csvFile)
        strKeys = dnaDB.fieldnames[1:]

    # read the first (and only) line from the dna sequence from the file
    with open(argv[2]) as dnafile:
        nnDnaChain = dnafile.readline()

    # create a dictionary with the strKey as key and the max amount of contiguous appearances of that strKey as value
    dnaAttr = {}
    for strKey in strKeys:
        if strKey in nnDnaChain:
            strReps = findall(r"(?:{})+".format(strKey), nnDnaChain)
            maxStrRep = max(strReps, key=len)
            # they are converted to string for to match the csv
            dnaAttr[strKey] = str(maxStrRep.count(strKey))

    # iterate all persons to find who matches the dnaAttr
    with open(argv[1], newline='') as csvFile:
        dnaDB = DictReader(csvFile)
        for personAttr in dnaDB:
            if {k: v for k, v in personAttr.items() if k != 'name'} == dnaAttr:
                print(personAttr['name'])
                return
        print("No match")
        return


main()

# Old code I didn't want to get rid of

# find all the elements from strKeys that are present in the nnDnaChain string and split them in a list
# Example:
# converts 1.txt from this:
# AAGGTAAGTTTAGAATATAAAAGGTGAGTTAAATAGAATAGGTTAAAATTAAAGGAGATCAGATCAGATCAGATCTATCTATCTATCTATCTATCAGAAAAGAGTAAATAGTTAAAGAGTAAGATATTGAATTAATGGAAAATATTGTTGGGGAAAGGAGGGATAGAAGG
# into this:
# ['AGATC', 'AGATC', 'AGATC', 'AGATC', 'TATC', 'TATC', 'TATC', 'TATC', 'TATC', 'GAAA', 'GATA', 'AATG', 'GAAA', 'GAAA', 'GATA']

# splitted_nnDnaChain = findall("{}|{}|{}|{}|{}|{}|{}|{}".format(*strKeys), nnDnaChain)

# find all the elements from strKeys that are present (in a repeated fashion or not) in the nnDnaChain string and split them in a list
# Example:
# converts 1.txt from this:
# AAGGTAAGTTTAGAATATAAAAGGTGAGTTAAATAGAATAGGTTAAAATTAAAGGAGATCAGATCAGATCAGATCTATCTATCTATCTATCTATCAGAAAAGAGTAAATAGTTAAAGAGTAAGATATTGAATTAATGGAAAATATTGTTGGGGAAAGGAGGGATAGAAGG
# into this:
# ['AGATCAGATCAGATCAGATC', 'TATCTATCTATCTATCTATC', 'GAAA', 'GATA', 'AATG', 'GAAA', 'GAAA', 'GATA']
# notice there are some repeated elements (like 'GAAA', 'GAAA') these two shouldn't be toghether as they appear separated in the original string

# splitted_nnDnaChain = findall(r"(?:{})+|(?:{})+|(?:{})+|(?:{})+|(?:{})+|(?:{})+|(?:{})+|(?:{})+".format(*strKeys), nnDnaChain)

# now we clean splitted_nnDnaChain by keeping only the patterns where an element appears repeated the most

# final_splitted_nnDnaChain = []
# for strKey in strKeys:
#     if strKey in nnDnaChain:
#         final_splitted_nnDnaChain.append(max([seq for seq in splitted_nnDnaChain if strKey in seq], key = len))

# if we want to do it in a generic way (without knowing how many elements strKeys has), the line below generates the same result as the line above
# splitted_nnDnaChain = [max(findall(r"(?:{})+".format(seq), nnDnaChain), key = len)for seq in strKeys if seq in nnDnaChain]