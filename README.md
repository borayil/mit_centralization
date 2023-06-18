# README.md

## How To Build

```
g++ -std=c++11 -Wall -Wextra -o program.out *.cpp
```

## How To Run

on Linux or Mac

```
./program.out
```

on Windows

```
program.exe
```

## Approach

For interactive demonstration of approach and offset calculation, please visit the following link which I created using GeoGebra for this challenge.
https://www.geogebra.org/calculator/bs8amyqz

We get our contact points w.r.t ourselves (the tools center which could be anywhere within pipe).
The contact points are evenly distributed at angles of `360 / number of fingers`.

Since we know the radius of the pipe, we can calculate the expected readings, expected contact points at same angles. Now we have expected contact points which we can compare to our actual readings for all the fingers at the same angles.

We calculate the difference between the vectors.

`vector_tool_to_expected_contact_point - vector_tool_to_contact_point`

We do this for all the fingers and get a vector for each finger. This vector is the offset for each finger.

The average of these vectors is the offset for the tool from the center of the pipe.
We can move our tool to the center of the pipe by applying this offset to the tool. We have to subtract the offset to reverse the effect of the offset.
