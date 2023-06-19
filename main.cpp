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
static const std::string slash = "\\";
#else
static const std::string slash = "/";
#endif

int main()
{

    // Setup
    mit caliper_tool(40, 254, 5);
    caliper_tool.load_readings("data" + slash + "data.txt");

    // Calculate offset vector and centralize readings
    point offset_vector = caliper_tool.calculate_offset_vector_of_sample(0);
    cout << "Tool offset vector from center of pipe: (" << offset_vector.x << ", " << offset_vector.y << ")" << endl;
    caliper_tool.centralize_readings(offset_vector);
    caliper_tool.show_readings(true);
    caliper_tool.save_centralized_readings("data" + slash + "data_centralized.txt");

    return 0;
}
