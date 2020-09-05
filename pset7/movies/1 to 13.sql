--1-----------------------------
SELECT title
  FROM movies
 WHERE year = 2008;

--2-----------------------------
SELECT birth
  FROM people
 WHERE name = 'Emma Stone';

--3-----------------------------
  SELECT title
    FROM movies
   WHERE year >= 2018
ORDER BY title;

--4-----------------------------
SELECT COUNT(*)
  FROM ratings
 WHERE rating = 10;

--5-----------------------------
  SELECT title
       , year
    FROM movies
   WHERE title LIKE 'Harry Potter%'
ORDER BY year;

--6-----------------------------
 SELECT AVG(rating)
   FROM ratings
  WHERE movie_id IN (
                      SELECT id
                        FROM movies
                       WHERE year = 2012
                    );

--other way
 SELECT AVG(r.rating)
   FROM ratings r
      , movies m
  WHERE r.movie_id = m.id
    AND m.year = 2012;

--7-----------------------------
  SELECT m.title
       , r.rating
    FROM ratings r
       , movies m
   WHERE r.movie_id = m.id
     AND m.year = 2010
ORDER BY rating DESC
       , title ASC;

--8-----------------------------
SELECT p.name
  FROM movies m
     , people p
     , stars s
 WHERE s.person_id = p.id
   AND s.movie_id = m.id
   AND m.title = 'Toy Story';

--9-----------------------------
  SELECT DISTINCT p.name
    FROM movies m
       , people p
       , stars s
   WHERE s.movie_id = m.id
     AND s.person_id = p.id
     AND m.year = 2004
ORDER BY p.birth;

--10-----------------------------
SELECT DISTINCT p.name
  FROM people p
     , directors d
     , ratings r
 WHERE d.movie_id = r.movie_id
   AND d.person_id = p.id
   and r.rating >= 9;

--11-----------------------------
  SELECT m.title
    FROM people p
       , movies m
       , ratings r
       , stars s
   WHERE m.id = r.movie_id
     AND m.id = s.movie_id
     AND s.person_id = p.id
     AND p.name = 'Chadwick Boseman'
ORDER BY r.rating DESC
   LIMIT 5;

--12-----------------------------
SELECT m.title
  FROM movies m
     , stars s1
     , stars s2
     , people p1
     , people p2
 WHERE m.id  = s1.movie_id
   AND s1.movie_id = s2.movie_id
   AND s1.person_id = p1.id
   AND s2.person_id = p2.id
   AND p1.name = 'Johnny Depp'
   AND p2.name = 'Helena Bonham Carter';

--other way
SELECT title
  FROM movies
 WHERE id IN (SELECT s1.movie_id
                FROM stars s1
                   , stars s2
               WHERE s1.movie_id = s2.movie_id
                 AND s1.person_id = (SELECT id
                                       FROM people
                                      WHERE name = 'Johnny Depp'
                                    )
                 AND s2.person_id = (SELECT id
                                       FROM people
                                      WHERE name = 'Helena Bonham Carter'
                                    )
             );

--other way
SELECT title
  FROM movies
 WHERE id IN (SELECT movie_id
                FROM stars
               WHERE person_id = (SELECT id
                                    FROM people
                                   WHERE name = 'Johnny Depp'
                                 )
                 AND movie_id IN SELECT movie_id
                                   FROM stars
                                  WHERE person_id = (SELECT id
                                                       FROM people
                                                      WHERE name = 'Helena Bonham Carter'
                                                    )
             );


--other way
WITH t0 AS (SELECT p.name
                 , m.title
                 , m.id
              FROM people p
                 , movies m
                 , stars s
             WHERE m.id = s.movie_id
               AND s.person_id = p.id
               AND p.name IN ('Johnny Depp', 'Helena Bonham Carter')
           )
      SELECT t1.title
        FROM t0 AS t1
           , t0 AS t2
       WHERE t1.id = t2.id
         AND t1.name = 'Johnny Depp'
         AND t2.name = 'Helena Bonham Carter';

--13-----------------------------
SELECT DISTINCT p1.name
  FROM people p1
     , people p2
     , stars s1
     , stars s2
 WHERE s1.movie_id = s2.movie_id
   AND s1.person_id = p1.id
   AND s2.person_id = p2.id
   AND p1.name != 'Kevin Bacon'
   AND p2.name = 'Kevin Bacon'
   AND p2.birth = 1958;
 
--other way
SELECT name
  FROM people
 WHERE name != 'Kevin Bacon'
   AND id IN (SELECT s1.person_id
                FROM stars s1
                   , stars s2
               WHERE s1.movie_id = s2.movie_id
                 AND s2.person_id = (SELECT id
                                       FROM people
                                      WHERE name = 'Kevin Bacon'
                                        AND birth = 1958
                                    )
             );

--other way
SELECT name
  FROM people
 WHERE name != 'Kevin Bacon'
   AND id IN (SELECT person_id
                FROM stars
               WHERE movie_id IN (SELECT movie_id
                                    FROM stars
                                   WHERE person_id = (SELECT id
                                                        FROM people
                                                       WHERE name = 'Kevin Bacon'
                                                         AND birth = 1958
                                                     )
                                 )
             );

--other way
WITH t1 AS (SELECT id
              FROM people
             WHERE name = 'Kevin Bacon'
               AND birth = 1958
           )
   , t2 AS (SELECT s.movie_id
              FROM stars s
                 , t1
             WHERE s.person_id = t1.id
           )
   , t3 AS (SELECT s.person_id
              FROM stars s
                 , t2
             WHERE s.movie_id = t2.movie_id
           )
      SELECT DISTINCT p.name
        FROM people p
           , t3
       WHERE p.name != 'Kevin Bacon'
         AND p.id = t3.person_id;

--other way
WITH t0 AS (SELECT p.name
                 , s.movie_id
                 , p.birth
              FROM people p
                 , stars s
             WHERE s.person_id = p.id
           )
      SELECT DISTINCT name
        FROM t0
       WHERE movie_id IN (SELECT movie_id
                            FROM t0
                           WHERE name = 'Kevin Bacon'
                             AND birth = 1958
                         )
         AND name != 'Kevin Bacon';

--other way         
WITH t0 AS (SELECT p.name
                 , s.movie_id
                 , p.birth
              FROM people p
                 , stars s
             WHERE s.person_id = p.id
           )
      SELECT DISTINCT a.name
        FROM t0 AS a
           , t0 AS b
       WHERE a.name != 'Kevin Bacon'
         AND a.movie_id = b.movie_id
         AND b.name = 'Kevin Bacon'
         AND b.birth = 1958;
