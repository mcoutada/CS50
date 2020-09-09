"""
HOUSES: import.py

Usage:
    python import.py characters.csv

Check insertion through command line:
cd ~/pset7/houses
sqlite3
.header on
.mode column
.open "students.db"
SELECT * FROM students WHERE last IN ('Longbottom','Weasley');
"""

from csv import DictReader
from sys import argv
from sqlite3 import connect


def main():

    # argv is an arguments list where argv[0] = 'import.py' and sys.argv[1] = 'characters.csv'
    if len(argv) != 2:
        print("Usage: python import.py characters.csv")
        return

    with connect("students.db") as conn:

        # Empty the table in case there's records from previous runs
        conn.cursor().execute("DELETE FROM students;")

        # csvFile headers = name,house,birth
        with open(argv[1], "r") as csvFile:

            housesDB = DictReader(csvFile)

            for row in housesDB:
                house = row["house"]
                born = row["birth"]
                name = row["name"].split()
                firstName = name[0]
                middleName = "NULL" if name[1] == name[-1] else "'" + name[1]  + "'"
                lastName = name[-1]

                # Triple quotes (both varieties, """ and ''' are permitted) allow the string to contain line breaks
                sql = f"""INSERT INTO students ( first
                                               , middle
                                               , last
                                               , house
                                               , birth
                                               )
                                        VALUES ( '{firstName}'
                                               ,  {middleName}
                                               , '{lastName}'
                                               , '{house}'
                                               ,  {born}
                                               );
                       """
                conn.cursor().execute(sql)
            conn.commit()
    return


main()