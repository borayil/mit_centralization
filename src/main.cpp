/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: Main runner file for the challenge solution.
 */
#include <iostream>
#include <vector>
#include "mit.h"    // MIT class
#include "config.h" // Configurations of tool
using namespace std;

int main()
{
    mit caliper_tool = mit(NO_OF_FINGERS, PIPE_DIAMETER_MM, DISTANCE_BETWEEN_SAMPLES_MM);
    caliper_tool.load_readings(PATH_TO_DATA);

    point offset_vector = caliper_tool.calculate_offset_vector_of_sample(0);
    caliper_tool.centralize_readings(offset_vector);

    caliper_tool.save_centralized_readings(PATH_TO_TRANSFORMED_DATA);
    return 0;
}
