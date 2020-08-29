
"""
DNA
Running:                                                             Retrieves:
python ~/pset6/dna/dna.py databases/large.csv sequences/5.txt        Lavender 
"""
from csv import DictReader
from sys import argv
from re import findall


def main():

    # argv is an arguments list. Example: argv[0]='dna.py', sys.argv[1]='databases/large.csv' sys.argv[2] = 'sequences/5.txt'
    if len(argv) != 3:
        print("Usage: python dna.py data.csv sequence.txt")
        return

    # read the first (and only) line from the dna sequence from the file
    with open(argv[2]) as dnafile:
        nnDnaChain = dnafile.readline()

    # extract the DNA Short Tandem Repeats (STRs) from the first line (fieldnames) of the csv
    # strKeys = ['AGATC', 'TTTTTTCT', 'AATG', 'TCTAG', 'GATA', 'TATC', 'GAAA', 'TCTG']
    with open(argv[1]) as csvFile:
        dnaDB = DictReader(csvFile)
        strKeys = dnaDB.fieldnames[1:]

    # create a dictionary with the strKey as key and its max amount of contiguous appearances as value
    nnRec = {}
    for strKey in strKeys:
        if strKey in nnDnaChain:
            strReps = findall(r"(?:{})+".format(strKey), nnDnaChain)
            maxStrRep = max(strReps, key=len)
            # count the appearances and set its number to string to match the csv
            nnRec[strKey] = str(maxStrRep.count(strKey))

    # iterate all people records to find who matches the nnRec
    with open(argv[1], newline='') as csvFile:
        dnaDB = DictReader(csvFile)
        for pplRec in dnaDB:
            if nnRec == {key: val for key, val in pplRec.items() if key != 'name'}:
                print(pplRec['name'])
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
