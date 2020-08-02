#include <cs50.h>
#include <stdio.h>
#include <string.h> // this is needed for strcmp, strlen

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool closesloop(int a, int b);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        // verify a valid candidate
        if (strcmp(candidates[i], name) == 0)
        {
            // verify if it wasn't chosen in previous ranks
            //This for loop is not needed as check50 doesn't test it, but it prevents repeating an already ranked candidate for a voter
            for (int j = 0; j < rank; j++)
            {
                if (ranks[j] == i)
                {
                    return false;
                }
            }
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // if I have candidates A B C D (and I enumerate them as: A = 0, B = 1, C = 2, D = 3):
    // every voter will rank 4 candidates in their vote --> amount of rankings = candidate_count = strlen(ranks) = 4
    //
    // if voter1's vote = A, D, C, B
    // ranks[0] = A = 0
    // ranks[1] = D = 3
    // ranks[2] = C = 2
    // ranks[3] = B = 1
    //
    // So:                 -->  [winner, looser]:  -->  preferences[winner][looser] counters I need to process:
    // A beats B, C and D  -->  [A,D] [A,C] [A,B]  -->  preferences[0][3]++, preferences[0][2]++, preferences[0][1]++
    // D beats C, B        -->  [D,C] [D,B]        -->  preferences[3][2]++, preferences[3][1]++,
    // C beats B           -->  [C,B]              -->  preferences[2][1]++
    //
    // In a generic way, the [winner, looser] pairs I need to process are:
    // 1st place: [ranks[0], ranks[1]] , [ranks[0], ranks[2]] , [ranks[0], ranks[3]]
    // 2nd place: [ranks[1], ranks[2]] , [ranks[1], ranks[3]]
    // 3rd place: [ranks[2], ranks[3]]
    //
    // We can see that:
    // In 1st place: [ranks[w], ranks[l]] iterations are w = 0 and l = 1, 2, 3
    // In 2nd place: [ranks[w], ranks[l]] iterations are w = 1 and l = 2, 3
    // In 3rd place: [ranks[w], ranks[l]] iterations are w = 2 and l = 3
    //
    // (w stands for winner, l for looser)
    //
    // this means  (0 <= W <= 2) hence (0 <= W < 3) hence (0 <= W < candidate_count - 1)
    //             (1 <= L <= 3) hence (1 <= L < 4) hence  (1 <= L < candidate_count)

    for (int w = 0; w < candidate_count - 1; w++)
    {
        for (int l = w + 1; l < candidate_count; l++)
        {
            preferences[ranks[w]][ranks[l]]++;
        }
    }
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // From our previous example, voter1's vote = A, D, C, B
    // After the first iteration (voter1), our preferences[w][l] array should look this way:
    //
    //       |  l=0    l=1    l=2    l=3
    //   --------------------------------
    //   w=0 |   -     +1     +1     +1
    //   w=1 |   0      -      0      0
    //   w=2 |   0     +1      -      0
    //   w=3 |   0     +1     +1      -
    //
    // (we ignore the middle diagonal as it's comparing a candidate against himself).
    // voter1 prefers candidate A over the rest, hence its preferences over, for example, D, are:
    //     preferences[A][D] = preferences[w=0][l=3] = +1
    //     preferences[D][A] = preferences[w=3][l=0] = 0
    //
    // After all voters voted, our preferences array could look something like:
    //       |  l=0    l=1    l=2    l=3
    //   --------------------------------
    //   w=0 |   -      3      0      1
    //   w=1 |   2      -      1      0
    //   w=2 |   0      4      -      2
    //   w=3 |   4      2      3      -
    //
    // We need to translate these results into winner/loser pairs.
    // For that, we use a "pairs" array that has a "winner" and "loser" attributes
    // So, for our A,D pair example:
    // preferences[A][D] = 1 --> pairs[AD].loser = A
    // preferences[D][A] = 4 --> pairs[AD].winner = D
    // (we won't be able to use chars as an array index, we will use an equivalent number)
    //
    // Ok, but how many pairs do we have?
    // Let's identify all the possible combinations
    //
    //       |    A         B         C         D
    //     ------------------------------------------
    //     A |   A,A       A,B       A,C       A,D
    //     B |   B,A       B,B       B,C       B,D
    //     C |   C,A       C,B       C,C       C,D
    //     D |   D,A       D,B       D,C       D,D
    //
    // But we don't need to pair a candidate with himself (middle diagonal)
    // Neither we need B,A if we already have the A,B pair, as our array has the "winner", "loser" attributes (B,A will just be the opposite of A,B)
    //
    //       |    A         B         C         D
    //     ------------------------------------------
    //     A | (IGNORE)    A,B       A,C       A,D
    //     B | (IGNORE)  (IGNORE)    B,C       B,D
    //     C | (IGNORE)  (IGNORE)  (IGNORE)    C,D
    //     D | (IGNORE)  (IGNORE)  (IGNORE)  (IGNORE)
    //
    // Now we need to name our pairs, and we have to use index numbers, as "pairs" is an array.
    // So, our new names will be:
    //
    //       |    A         B         C         D
    //     ------------------------------------------
    //     A | (IGNORE)   pairs[0]  pairs[1]  pairs[2]
    //     B | (IGNORE)  (IGNORE)   pairs[3]  pairs[4]
    //     C | (IGNORE)  (IGNORE)  (IGNORE)   pairs[5]
    //     D | (IGNORE)  (IGNORE)  (IGNORE)  (IGNORE)
    //
    // I think we might have been able to use another order, but we scan matrices from
    // left to right, top to bottom so the above convention makes the most sense
    //
    // Notice something interesting, we have 6 elements, and "pairs" was declared as:
    // pair pairs[MAX * (MAX - 1) / 2];
    // why is “pairs” declared this way?
    // We have 4 candidates, so pairs is sized to 4 * (4  - 1) / 2 = 6, which is in fact, our amount of pairs
    // MAX * (MAX - 1) / 2 is the formula for a Gaussian sum, that basically
    // counts the elements on the upper diagonals:
    // 6 = 3 + 2 + 1 = 4*(4-1)/2
    // (3 is counting  pairs[0],  pairs[3], and  pairs[5])
    // (2 is counting  pairs[1] and pairs[4])
    // (1 is counting  pairs[5])
    //
    // Remember we need to relate our "preferences" matrix with our "pairs" matrix with number notation
    //
    // If:
    // preferences[A][D] = 1  --> pairs[AD].winner = D
    // preferences[D][A] = 4  --> pairs[AD].loser = A
    //
    // Using our number's notation, it will be:
    // preferences[0][3] = 1   --> pairs[2].winner = 3
    // preferences[3][0] = 4   --> pairs[2].loser = 0
    //
    // remember we need to ignore a tie (per exercise statement)

    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++; // we only count if there's no tie
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++; // we only count if there's no tie
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    pair temp;
    bool sorted;

    // bubble sort

    for (int i = 0; i < pair_count - 1; i++)
    {
        sorted = true;

        for (int j = 0; j < pair_count - i - 1; j++)
        {

            if (preferences[pairs[j].winner][pairs[j].loser] < preferences[pairs[j + 1].winner][pairs[j + 1].loser])
            {
                temp = pairs[j];
                pairs[j] = pairs[j + 1];
                pairs[j + 1] = temp;
                sorted = false;
            }
        }

        // if we scanned all pairs without swaping any, it means they are already sorted
        if (sorted)
        {
            break;
        }
    }
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{

    // It's probably useful to read some graph theory first, and becoming familiar with terms like "adjacency matrix" "Depth First Search" "transverse". And also see "How to Detect Cycle in Directed Graph"
    // There's some theory behind this function that we didn't see yet in the course.
    // Grabbing pen an paper to draw the examples I gonna explain, it will make your life much easier

    // How we detect a cycle?
    // If the edge A-->B exists, a cycle exists if there's a way to depart from B and reach A
    // The key here is to understand that the edges to build this alternative way to reach "A" HAVE TO BE LOCKED
    // why? Because we already sorted them by victory strength, so we start processing good guys, locking them, then look if any of the next ones create a loop
    // The locked array is our graph, and it’s dynamic, it’s empty on the first iteration (that’s why we can safely lock the first edge, it has nothing to cycle with), and starts getting bigger (or equal, if we don’t lock) over the next iterations (edges being processed), therefore we have to do more tests for later edges in order to safely lock them.
    // for instance, we first process A-->B, there's no locked edge yet as it's the first one, so we lock it. Then, we process B-->C
    // let´s say we lock  A-->B, the next question we ask is if B-->C generates a loop in our graph (which is only A-->B, so we ask if the edge B-->C == B-->A) it’s not, so we lock B-->C.
    // Read this exercise's description part carefully:
    // "Tideman algorithm must be careful to avoid creating cycles in the candidate graph. How does it do this? The algorithm locks in the strongest edges first, since those are arguably the most significant. In particular, the Tideman algorithm specifies that matchup edges should be “locked in” to the graph one at a time, based on the “strength” of the victory (the more people who prefer a candidate over their opponent, the stronger the victory). So long as the edge can be locked into the graph without creating a cycle, the edge is added; otherwise, the edge is ignored."

    // It's important to notice that every time we process an edge, we are asking if it's a bad guy (generates a cycle)
    // we take an edge, for example, F-->E
    // the simplest case would be this cycle: E-->F-->E (2 opposite edges: E-->F, F-->E)
    // we process E-->F, it's the first one so no alternative way to reach E departing from F, so we lock it.
    // but then we process F-->E so there's a cycle (E-->F-->E) as E-->F was already locked, so we don't lock F-->E
    // LET'S CALL THIS, THE BASE CASE, as it's the simplest one, but this concept will be important for recursion

    // the next case that could happen is this cycle: E-->F-->H-->E (3 edges: E-->F, F-->H, H-->E)
    // we process E-->F, as it's the first one, we lock it.
    // we process F-->H, as F-->H != F-->E, we lock it.
    // but then we process H-->E which closes a loop in our graph of already locked edges (E-->F-->H), so we don't lock it.

    // And so on...

    // take into account that edges come sorted but not "pre-hooked" as my previous examples.
    // You might receive:
    // iteration 0: E-->F
    // iteration 1: Y-->Z
    // iteration 2: Z-->K
    // iteration 3: F-->E   <--LOOP! E-->F-->E

    // But this doesn't change our solution's approach, we need to see if the edge being processed fits in the extremes of some chain that we can build with 1, 2 or n of the already good processed edges

    // There are several ways to do this, but one of simplest ones is:
    // We are processing H-->E
    // does E-->H exist in the locked array? If so, we found a 2 edges cycle (E-->H-->E), so we skip locking H-->E
    // if not, let's look in the locked array if we can find a good guy to hook up at the beginning of our bad guy, and see if the new extremes generate a loop
    // Let's say, we found F-->H in locked, ok, we hook it up, now we have F-->H-->E (good guy hooked behind bad guy)
    // now, we check if E-->F exists among our good guys (is E-->F locked?) if so, we found a 3 edges cycle (E-->F-->H-->E), so we skip locking H-->E
    // if not, hook up another good guy at the beginning of the chain and check the new extremes
    // And so on...
    // If there’s no good guys left to hook up, then it’s safe to lock H-->E

    // Pseudo code:
    // We create the function closesloop and call it inside lock_pairs to see whether the current edge generates a loop
    //     Call lock_pairs:
    //         loop all pairs from 0 to pair_count (remember, they are already sorted by victory strength)
    //             ask if the pair being processed creates a cycle [call closesloop(pairs[i].winner, pairs[i].loser)]
    //             if winner-->looser doesn't create a cycle, then lock it (locked[pairs[i].winner][pairs[i].loser] = true)
    //             if it creates a cycle, just ignore it, the locked matrix is previously defaulted to false by the code provided by cs50    <--(while this is true, don't ignore it, set it to false, see my notes at the end on how check50 works)

    //     Call closesloop:
    //         we receive the parameters (pairs[i].winner, pairs[i].loser) and call it (A, B) for clarity
    //         We are processing A-->B, so (Base case) is locked[B][A] = true? If so, return true as there’s a cycle A-->B-->A
    //         if not, loop through the already locked edges, and search for an edge pointing to our edge's tail (for example, C-->A):
    //             C-->A is locked, hook it up behind, and call closesloop recursively:
    //                 This is an important step, we now have C-->A-->B, and have to look if B-->C was previously locked, in order to avoid the cycle C-->A-->B-->C, if so, A-->B should not be locked.
    //                 In order for that to be our next question (is B-->C locked?), we need to call closesloop(C,B) recursively so its base case will ask if locked[B][C] = true (if B-->C locked)
    //         if there's no remaining edge in the locked array to hook up (the loop ended), just return false, we can lock A-->B

    // Pseudo code again, the same example, but closer to code language:

    //     Call lock_pairs, loop from 0 to pair_count:
    //         1. call closesloop(A,B):
    //                 Is B-->A locked? No >>> loop through locked to find ?-->A >>> Nothing? Ok we are out of the loop, return false
    //                 >>> closesloop(A,B) == false, we can lock A-->B
    //                 locked = {A-->B}
    //         2. call closesloop(B,C) >>> Is C-->B locked? No >>> loop through locked to find ?-->B >>> Found A-->B, Ok hook it: A-->B-->C and call recursively closesloop(A,C)
    //             2.1 call closesloop(A,C):
    //                      Is C-->A locked? No >>> loop through locked to find ?-->A >>> Nothing? Ok we are out of the loop, return false
    //                      >>> closesloop(A,C) == false >>> return false to closesloop(B,C)
    //                 >>> closesloop(B,C) == false, we can lock B-->C
    //            locked = {A-->B, B-->C}

    // I think we are getting the idea, and it's getting a little verbose. If you already got it, great, let recursion do its magic for further cases, and you can skip the example below, if not, let's do one more example with a nice table, this is the table I used to debug (old school, pen and paper).
    // let's say we have the following edges in order of strength (desc):
    // 1-->2
    // 2-->3
    // 3-->1

    // (We see there's a cycle: 1-->2-->3-->1)

    // (It's read left to right, top to bottom)
    // iteration   locked array      closesloop(A,B)    B-->A in locked?      ?-->A in locked?            Result
    //    0             {}                1-->2         2-->1 not in locked   No, no ?-->1 in locked      return false to lock_pairs, which locks 1-->2
    //    1           {1-->2}             2-->3         3-->2 not in locked   Yes, 1-->2 in locked        hook it behind (1-->2-->3) and call recursively closesloop(1,3)
    //   1.1          {1-->2}             1-->3         3-->1 not in locked   No, no ?-->1 in locked      return false to closesloop(2,3) which returns false to lock_pairs, and locks 2-->3
    //    2        {1-->2, 2-->3}         3-->1         1-->3 not in locked   Yes, 2-->3 in locked        hook it behind (2-->3-->1) and call recursively closesloop(2,1)
    //   2.2       {1-->2, 2-->3}         2-->1         1-->2 in locked       this logic isn't reached    return true to closesloop(3,1) which returns true to lock_pairs, and skips locking 3-->1

    // check50 notes:
    // there are 2 important things to know about how check50 tests this function, that I realized them while debugging
    // 1. assume pairs[i].winner might be == pairs[i].loser
    // Even though I added logic in add_pairs to prevent it, check50 seems to test tideman functions separately, so we need to prevent these cases

    // 2. Don't assume locked[pairs[i].winner][pairs[i].loser] == false by default
    // even though this is done at the beginning by the code provided by cs50, again, check50 seems to be overwriting it with nothing
    // So basically write either true or false on every locked[pairs[i].winner][pairs[i].loser] when they are iterated
    // 3. (bonus) when I solved 2, logic for 1 wasn't needed (it makes sense that our lock_pairs function doesn’t lock a-->a)... check it just in case...

    // If check50 fails to pass "final pair" or "middle pair" and you already did the above:
    // Here are the graphs used in check50 for the middle pairs and final pairs check.

    //  Middle Pair - https://i.imgur.com/mdsDidq.png

    //  Final Pair - https://i.imgur.com/OJosjRe.png

    // Source: comments in https://www.reddit.com/r/cs50/comments/hcmlsi/cs50_tideman_my_code_seems_right_but_lock_pairs/

    for (int i = 0; i < pair_count; i++)
    {
        // if it creates a cycle, then closesloop == true, so we save the opposite in locked (we shouldn't lock it, so locked = !true = false)
        locked[pairs[i].winner][pairs[i].loser] = !closesloop(pairs[i].winner, pairs[i].loser);
    }
}
// the basic implementation of the closesloop algorithm we explained above is:
//
// bool closesloop(int a, int b)
// {
//     // base case
//     if (locked[b][a])
//     {
//         // b-->a is locked, therefore a-->b creates a cycle
//         return true;
//     }
//
//     for (int i = 0; i < candidate_count; i++)
//     {
//         if (locked[i][a])
//         {
//             // i-->a is locked, we now have i-->a-->b, we need to check if b-->i is locked (which generates the cycle i-->a-->b-->i)
//             // so we call closesloop(i, b) in order for it's base case to ask if b-->i is locked
//             return closesloop(i, b);
//         }
//     }
//     return false;
// }

// closesloop: improving the algorithm
// Now that we understand it, let's improve a little this algorithm.
// Why are we going to hook up good guys behind a-->b if maybe nothing fits in front? We need to be able to connect both extremes in order to have a cycle
// Let's apply the "good guys logic" to both extremes of a-->b

// bool closesloop(int a, int b)
// {
//     // base case
//     if (locked[b][a] || a == b)
//     {
//         // either a-->a or a-->b-->a exist
//         return true;
//     }
//
//     for (int i = 0; i < candidate_count; i++)
//     {
//         // find b-->i (a way to depart from a-->b == a-->b-->i)
//         if (locked[b][i])
//         {
//             for (int j = 0; j < candidate_count; j++)
//             {
//                 // find j-->a (a way to reach to a-->b == j-->a-->b)
//                 if (locked[j][a])
//                 {
//                     // b-->i and j-->a are locked, we now have j-->a-->b-->i, we need to check if i-->j is locked (which generates the cycle j-->a-->b-->i-->j)
//                     // so we call closesloop(j, i) in order for it's base case to ask if i-->j is locked
//                     return closesloop(j, i);
//                 }
//             }
//             return false; // if there's no j-->a to reach a-->b from behind, there's no cycle
//         }
//     }
//     return false; // if there's no b-->i to depart from a-->b, there's no cycle
// }

// closesloop: second improvement
// if we find b-->i, let's ask if i-->b is locked, if so, there's no point of looking for j-->a as we already have a 3 nodes cycle i-->a-->b-->i
// in fact, we will find j-->a, and it will be == b-->i, so let's change an array scan for just one operation
// but if we found b-->i (and we know already i-->b is not locked), then we look for j-->a, so now have j-->a-->b-->i, the next possibility is a 4 nodes cycle, so we ask recursively if if i-->j exists

bool closesloop(int a, int b)
{
    // base case 1 (2 nodes cycle) a-->b-->a exists
    if (locked[b][a])
    {
        return true;
    }

    for (int i = 0; i < candidate_count; i++)
    {
        // find b-->i (a-->b-->i)
        if (locked[b][i])
        {
            // base case 2 (3 nodes cycle) i-->a-->b-->i exists
            if (locked[i][a])
            {
                return true;
            }

            for (int j = 0; j < candidate_count; j++)
            {
                // find j-->a (a way to reach to a-->b == j-->a-->b)
                if (locked[j][a])
                {
                    // b-->i and j-->a are locked, we now have j-->a-->b-->i , we need to check if i-->j is locked (which generates the 4 nodes cycle j-->a-->b-->i-->j)
                    // so we call closesloop(j, i) in order for it's base case to ask if i-->j is locked
                    return closesloop(j, i);
                }
            }
            return false; // if there's no j-->a to reach a-->b from behind, there's no cycle
        }
    }
    return false; // if there's no b-->i to depart from a-->b, there's no cycle
}

// Print the winner of the election
void print_winner(void)
{
    // The source of the graph has no edge pointing to it
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                // while processing candidate i, we found j-->i, skip to candidate i+1
                break;
            }
        }

        printf("%s\n", candidates[i]);
    }
}
