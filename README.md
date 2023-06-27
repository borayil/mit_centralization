# MIT Centralization

![3d](https://github.com/borayil/mit_centralization/assets/56681820/6a3be640-b8a5-4fe7-ae9b-eaf672fbbc2d)

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

Transformed data will be saved in `data/transformed_data.txt` after running the program.

### Visualize

I implemented plotting to see how the data is being transformed and how to make sense of it. To visualize the results, go into the `visualization` directory. Running the python scripts

```
python visualization.py
python gif.py
```

will generate visualizations within `visualization/images`.

### Problem

Given MIT configuration and pipe radius readings, find the offset of the tool from the center of the pipe and centralize the reading distances by re-calculating the distances to the contact points as if they started from the pipe center. Without centralization, the readings are not relative to the pipe center, and thus the readings are not accurate / meaningful. Current data set provided is for a MIT with 40 fingers, 254mm pipe diameter and 5mm difference between samples. Each line is a sample with readings from all fingers in same order throughout samples.

### Offset calculation and centralization

Using a combination of trigonometry, vectors and algebra, we can calculate the offset of the tool and centralize then readings. Have to be careful with the signs of the vectors and angles, and also the relative origin at any given point. We consider each sample a 2D slice of the pipe, and we use that for our calculations.

- Fingers are distributed evenly around the circle, with the first finger at 0 degrees.
- All fingers have a corresponding finger on the opposite side of the tool.
- We know the expected radius of the circle.
- Using the angle between minimum reading and x-axis, max and min readings, we can calculate the offset of the tool.

1. Distribute fingers evenly around the circle, with the first finger at 0 degrees. I pre-calculate cos and sin values for each finger to avoid calculating them every time
2. Calculate offset vector by getting max and min distance reading fingers. Getting contact point vector of minimum reading (where we are closer to pipe wall reading), and then getting the angle between that contact point vector and the x-axis. We can now find how offset we are from (0,0) by multiplying the difference between max and min readings with the cos and sin of the angle we calculated, respectively to find the x and y offset.
3. Initial readings were w.r.t. tool. Now we can centralize the readings by recalculating distances to contact points by getting the pipe center point w.r.t. tool which is -offset vector. We can then calculate the distance from this pipe center to the contact point, which gives us the re-calculated distance reading (as if the lines started at (0,0))

The data is loaded and saved line by line and number by number using file streams.

### Code Structure

For explanation of code, I have commented on the header files which summarizes members, and also on the .cpp files on specific parts.

- `main.cpp` entry point of the program and solution. It makes use of the class implemented to load data, run the algorithm and save the results. The main function is here
- `mit` implementation of the mit class.
- `config` configuration file for the program. It contains the number of fingers, the expected radius of the pipe, distance between samples and the file paths. Also, the slash is defined here to be used in file paths.
- `utils` utility and helper functions

### Results

From the dataset, we expect the pipe to be shaped like an old barrel. So in the top and bottom, we expect close to actual radius, while in the middle we expect a belly like shape.

Visualizing the initial readings will also produce a barrel shape **however** the readings are not centralized, so they are relative to the tool center, and not the pipe center. This means that the readings are not accurate / meaningful. We can clearly see by the colormap that the deviation from expected radius is not clearly visible, and the readings are spread out over a wide range.

Visualization of the centralized readings show a perfect color map on the pipe shaped like an old barrel as expected! Very nice. The plot of the finger readings over samples also shows how the readings are centralized. Before they are spread out over a wide range (because of very short readings due to tool offset), but after centralization, we can see how they start from expected radius and spread out evenly over the middle of the pipe as expected. The readings are not spread out over a wide range anymore, and are much more meaningful since they are relative to the pipe center and the pipe radius.
