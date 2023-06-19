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

struct point
{
    double x; // x component
    double y; // y component
};

struct reading
{
    double distance;             // Distance reading from the finger
    point contact_point;         // Contact point w.r.t. the tool
    double centralized_distance; // Distance reading from the finger after centralization
    bool is_centralized = false; // Flag to indicate if the reading is centralized
    // double angle;             // Angle of the finger w.r.t. the center of the pipe
};

class mit
{
private:
    int no_of_fingers;                          // Number of fingers
    double pipe_diameter;                       // Pipe diameter in mm
    double pipe_radius;                         // Pipe radius in mm
    double distance_between_samples_mm;         // Distance between samples (depths) in mm
    std::vector<double> cos_values;             // Pre-calculated cos values for each finger
    std::vector<double> sin_values;             // Pre-calculated sin values for each finger
    std::vector<std::vector<reading>> readings; // Rows are samples, columns are finger readings
public:
    mit(const int no_of_fingers,
        const double pipe_diameter_mm,
        const double distance_between_samples_mm);              // Constructor
    void load_readings(const std::string filename);             // Load readings from a file
    void save_centralized_readings(const std::string filename); // Save centralized readings to a file
    point calculate_offset_vector_of_sample(const int depth);   // Calculate offset vector of tool from the center of the pipe
    void calculate_contact_points();                            // Calculate contact points of each finger
    void centralize_readings(const point offset_vector);        // Centralize readings with given tool offset
    void show_readings(const bool show_centralized_readings);   // Show initial and centralized readings
    double calculate_distance(const point a, const point b);    // Calculate distance between two points
};

#endif // __CALIPER_TOOL_H__