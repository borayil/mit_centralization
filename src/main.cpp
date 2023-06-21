/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: This is the main file where we run our program for
 * loading, centralizing, and saving finger radius readings of MIT.
 * https://www.geogebra.org/calculator/bs8amyqz
 */
#include <iostream>
#include <vector>
#include "mit.h"    // MIT class
#include "config.h" // Configurations of tool
using namespace std;

int main()
{
    mit caliper_tool(NO_OF_FINGERS, PIPE_DIAMETER_MM, DISTANCE_BETWEEN_SAMPLES_MM);
    caliper_tool.load_readings(PATH_TO_DATA);
    point offset_vector = caliper_tool.calculate_offset_vector_of_sample(0);
    caliper_tool.centralize_readings(offset_vector);
    caliper_tool.save_centralized_readings(PATH_TO_DATA_CENTRALIZED);
    return 0;
}
