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
SELECT * FROM students WHERE last IN ('Longbottom','Weasley');
"""

from csv import DictReader
from sys import argv
from sqlite3 import connect


def main():

    # argv is an arguments list. Example: argv[0] = 'import.py', sys.argv[1] = 'characters.csv'
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
                name = row["name"].split()
                middleName = "".join(name[1:-1])
                middleName = "NULL" if middleName is None else "'" + middleName  + "'"

                # Triple quotes (both varieties, """ and ''' are permitted) allow the string to contain line breaks
                sql = f"""INSERT INTO students ( first
                                               , middle
                                               , last
                                               , house
                                               , birth
                                               )
                                        VALUES ( '{name[0]}'
                                               ,  {middleName}
                                               , '{name[-1]}'
                                               , '{row["house"]}'
                                               ,  {row["birth"]}
                                               );
                       """
                conn.cursor().execute(sql)
            conn.commit()
    return


main()