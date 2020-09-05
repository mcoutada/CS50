"""
HOUSES: roster.py
Usage:
python roster.py [Slytherin/Ravenclaw/Gryffindor/Hufflepuff]
"""

from sys import argv
from sqlite3 import connect


def main():

    # argv is an arguments list. Example: argv[0] = 'import.py', sys.argv[1] = 'characters.csv'
    if len(argv) != 2:
        print("Usage: python import.py characters.csv")
        return

    with connect('students.db') as conn:

        # Triple quotes (both varieties, """ and ''' are permitted) allow the string to contain line breaks
        sql = f"""  SELECT CASE WHEN middle is NULL
                           THEN
                               first || ' ' || last
                           ELSE
                               first || ' ' || middle || ' ' || last
                           END
                         , birth
                      FROM students
                     WHERE house = '{argv[1]}'
                  ORDER BY last, first
               """

        students = conn.execute(sql)

        # Iterate through all the students
        for row in students:
            print(f"{row[0]}, born {row[1]}")

    return


main()


# solving them middle name issue from python:

    # with connect('students.db') as conn:

    #     # Triple quotes (both varieties, """ and ''' are permitted) allow the string to contain line breaks
    #     sql = """  SELECT first
    #                     , middle
    #                     , last
    #                     , birth
    #                  FROM students
    #                 WHERE house = '{hogwartsHouse}'
    #              ORDER BY last, first
    #           """.format(hogwartsHouse = argv[1])

    #     students = conn.execute(sql)

    #     # Iterate through all the students
    #     for row in students:
    #         name = " ".join(row[:3] if row[1] else row[:1] + row[2:3])
    #         birth = row[3]
    #         print("{}, born {}".format(name, birth))