# Paradigm MIT Centralization Challenge

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

### Explanation of centralization of readings

### Code

For explanation of code, I have commented on the header files which summarizes members. We rely on the mit class, which loads, processes and saves the data.

- `main.cpp` is the entry point of the program. It makes use of the mit class to load data, run the algorithm and save the results.
- `mit.cpp` is the implementation of the mit class.
- `utils.cpp` is where utilities not necessarily related to `mit` class are implemented.
