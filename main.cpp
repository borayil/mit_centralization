/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: This is the main file where we run our program for
 * loading, centralizing, and saving readings from a MIT caliper.
 * Please see mit.cpp for more details on the MIT implementation.
 * Mathematical visualization of centralization approach:
 * https://www.geogebra.org/calculator/bs8amyqz
 */
#include <iostream>
#include <vector>
#include "mit.h"
using namespace std;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
static const string slash = "\\";
#else
static const string slash = "/";
#endif

const string PATH_TO_DATA = "data" + slash + "data.txt";

int main()
{

    // Setup
    int no_of_fingers = 40;
    double pipe_diameter_mm = 254;
    double distance_between_samples_mm = 5;
    mit caliper_tool(no_of_fingers, pipe_diameter_mm, distance_between_samples_mm);
    caliper_tool.load_readings(PATH_TO_DATA);

    // Process && Save
    point offset_vector = caliper_tool.calculate_offset_vector_of_sample(0);
    caliper_tool.centralize_readings(offset_vector);
    caliper_tool.save_centralized_readings("data" + slash + "data_centralized.txt");

    return 0;
}
