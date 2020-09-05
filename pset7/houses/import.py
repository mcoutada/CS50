"""
HOUSES: import.py

Usage:
    python import.py characters.csv

Check insertion:
cd ~/pset7/houses
sqlite3
.header on
.mode column
.open "students.db"
SELECT * FROM students WHERE last IN ('Crabbe','Weasley');
"""

from csv import DictReader
from sys import argv
from sqlite3 import connect


def main():

    # argv is an arguments list. Example: argv[0] = 'import.py', sys.argv[1] = 'characters.csv'
    if len(argv) != 2:
        print("Usage: python import.py characters.csv")
        return

    with connect('students.db') as conn:

        # Empty the table in case there's records from previous runs
        conn.cursor().execute('DELETE FROM students;')

        # csvFile headers = name,house,birth
        with open(argv[1], "r") as csvFile:
            housesDB = DictReader(csvFile)

            for pplRec in housesDB:
                firstName = ''.join(pplRec["name"].split()[:1])
                lastName = ''.join(pplRec["name"].split()[-1:])
                middleName = ''.join(pplRec["name"].split()[1:-1])
                middleName = 'NULL' if middleName is None else "'" + middleName  + "'" # "'" is equivalent to '\'' which are two valid ways to escape single quotes

        # Triple quotes (both varieties, """ and ''' are permitted) allow the string to contain line breaks
                sql = """INSERT INTO students ( first
                                              , middle
                                              , last
                                              , house
                                              , birth
                                              )
                                       VALUES ( '{}'
                                              , {}
                                              , '{}'
                                              , '{}'
                                              , {}
                                              );
                      """.format(firstName, middleName, lastName, pplRec["house"], pplRec["birth"])
                conn.cursor().execute(sql)
            conn.commit()
    return


main()
