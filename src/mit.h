/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: Header deffinitions for mit.cpp
 */
#ifndef __CALIPER_TOOL_H__
#define __CALIPER_TOOL_H__

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>
#include "utils.h"

struct point
{
    double x; // x component
    double y; // y component
};

struct reading
{
    size_t finger;               // Finger number
    double distance;             // Distance reading from the finger
    bool is_centralized = false; // Flag to indicate if the distance is centralized
};

class mit
{
private:
    int no_of_fingers;                          // Number of fingers
    double pipe_diameter;                       // Pipe diameter in mm
    double pipe_radius;                         // Pipe radius in mm
    double distance_between_samples_mm;         // Distance between samples (depths) in mm
    std::vector<double> finger_angles;          // Angles of each finger
    std::vector<double> cos_values;             // Pre-calculated cos values for each finger
    std::vector<double> sin_values;             // Pre-calculated sin values for each finger
    std::vector<std::vector<reading>> readings; // Rows are samples, columns are finger readings
public:
    mit(int no_of_fingers,
        double pipe_diameter_mm,
        double distance_between_samples_mm);                // Constructor
    void load_readings(std::string filename);               // Load readings from a file
    void save_centralized_readings(std::string filename);   // Save centralized readings to a file
    point calculate_offset_vector_of_sample(size_t sample); // Calculate offset vector of tool from the center of the pipe
    void calculate_contact_points();                        // Calculate contact points of each finger
    void centralize_readings(point offset_vector);          // Centralize readings with given tool offset
};

#endif // __CALIPER_TOOL_H__