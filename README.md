# MIT Centralization Challenge

### Build

```
g++ -std=c++11 -Wall -Wextra -o program.out *.cpp
```

### Run

on Linux or Mac

```
./program.out
```

on Windows

```
program.exe
```

### Explanation of offset calculation

[Offset Calculation 2D Sketch](https://www.geogebra.org/calculator/ur7apqkm)

Since it is given that offset is the same for all samples, we can calculate the offset from a single sample using the following steps, which rely on vectors and trigonometry.

1. Tool is our origin at (0, 0) and fingers are evenly distributed around the tool `2π / no_of_fingers`
2. For each finger, we calculate the contact point from origin using reading and finger angle.
3. For each finger, we calculate the expected contact point from origin using pipe radius and finger angle.
4. We calculate the vector from origin to contact point.
5. We calculate the vector from origin to expected contact point.
6. We calculate the difference between the vectors. This gives us the offset component of each finger.
7. The sum of these vectors divided by 2 is the offset of the tool from the center of the pipe. We divide by 2 to average the pull and push components of the offset. Some readings will push us away from the center and some will pull us towards the center.

### Explanation of centralization of readings

### Code

For explanation of code, I have commented on the header files which summarizes members. We rely on the mit class, which loads, processes and saves the data.

- `main.cpp` is the entry point of the program. It makes use of the mit class to load data, run the algorithm and save the results.
- `mit.cpp` is the implementation of the mit class.
- `utils.cpp` is where utilities not necessarily related to `mit` class are implemented.
