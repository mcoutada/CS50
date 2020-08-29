"""
readability exercise: python version
Design and implement a program, readability, that computes the Coleman-Liau index of the text.
Test: https://www.online-utility.org/english/readability_test_and_improve.jsp
"""

# main


def main():
    s = input("Text: ")
    letters = 0
    words = 1  # needs 1 to count the last word
    sentences = 0

# loop string
    for c in s:
        letters += c.isalpha()
        words += c in [' ', '\n']
        sentences += c in ['?', '!', '.']

    # Turn it into floating point division by making at least one operand a floating point number
    letters_avg = 100 * letters / words
    sentences_avg = 100 * sentences / words
    cl_idx = round(0.0588 * letters_avg - 0.296 * sentences_avg - 15.8)

    # print("letters: {}; words: {}; sentences: {}; cl_idx: {}".format(letters, words, sentences, cl_idx))

    if cl_idx < 1:
        print("Before Grade 1")
    elif cl_idx >= 16:
        print("Grade 16+")
    else:
        print("Grade {}".format(cl_idx))


main()
