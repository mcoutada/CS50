# mario function
def mario(n):
    for a in range(1, n+1):
        print(' '*(n-a), '#'*a + '  ' + '#'*a, sep='')


# mario function
while True:
    m = input("Height: ")
    n = int(m) if len(m) == 1 and m.isnumeric() else 0
    if int(n) in range(1, 9):
        break

mario(n)
