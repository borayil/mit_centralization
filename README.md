# Paradigm MIT Centralization Challenge

### Build

In the `src` directory

```
g++ -std=c++11 -Wall -Wextra -o program *.cpp
```

### Run

on Linux or Mac

```
./program
```

on Windows

```
program.exe
```

Transformed data will be saved in `data/transformed_data.txt`

To visualize the results, go into the `visualization` directory

```
python visualization.py
python gif_visualization.py
```

will generate visualizations within `visualization/images`.

### Problem

Given configuration and pipe radius readings, find the offset of the tool and centralize the reading distances. Without centralization, the readings are not relative to the pipe center, and thus the readings are not accurate / meaningful.

### Offset calculation and centralization

- Fingers are distributed evenly around the circle, with the first finger at 0 degrees.
- All fingers have a corresponding finger on the opposite side of the tool.
- We know the expected radius of the circle.
- Using the angle between minimum reading and x-axis, max and min readings, we can calculate the offset of the tool.

1. Distribute fingers evenly around the circle, with the first finger at 0 degrees. I pre-calculate cos and sin values for each finger to avoid calculating them every time
2. Calculate offset vector by getting max and min distance reading fingers. Getting contact point vector of minimum reading (where we are closer to pipe wall reading), and then getting the angle between that contact point vector and the x-axis. We can now find how offset we are from (0,0) by multiplying the difference between max and min readings with the cos and sin of the angle we calculated, respectively to find the x and y offset.
3. Initial readings were w.r.t. tool. Now we can centralize the readings by recalculating distances to contact points by getting the pipe center point w.r.t. tool which is -offset vector. We can then calculate the distance from this pipe center to the contact point, which gives us the re-calculated distance reading (as if the lines started at (0,0))

The data is saved line by line and number by number. It is saved the same way, using file streams.

### Algorithm Code

For explanation of code, I have commented on the header files which summarizes members.

- `main.cpp` entry point of the program and solution. It makes use of the class implemented to load data, run the algorithm and save the results.
- `mit` implementation of the mit class.
- `utils` utility and helper functions

### Visualization

In the directory `visualization`, there are two Python scripts, `visualization.py` and `gif_visualization.py` which will generate a visualization of the results to show the centralization. The first script will generate a static visualization of the results, while the second script will generate a gif animation of the results at `visualization/images/`

### Results

From the dataset, we expect a pipe shaped like an old barrel. So in the top and bottom, we expect close to actual radius, while in the middle we expect a belly like shape.

Visualization of the centralized readings show a nicely color mapped pipe shaped like an old barrel as expected! Very nice. The plot of the finger readings over samples also shows how the readings are centralized. Before they are spread out over a wide range (because of very short readings due to tool offset), but after centralization, we can see how they start from expected radius and spread out evenly over the middle of the pipe as expected.
