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
    double x;
    double y;
};

struct reading
{
    double distance;
    point contact_point;
};

class mit
{
private:
    int no_of_fingers;
    double pipe_diameter;
    double pipe_radius;
    double distance_between_samples_mm;
    std::vector<double> cos_values;             // Pre-calculated cos values for each finger
    std::vector<double> sin_values;             // Pre-calculated sin values for each finger
    std::vector<std::vector<reading>> readings; // Rows are samples, columns are finger readings

public:
    mit(const int no_of_fingers,
        const double pipe_diameter_mm,
        const double distance_between_samples_mm);
    void load_data(const std::string filename);
    point calculate_offset_vector();
    void centralize_readings(point offset_vector);
};

#endif // __CALIPER_TOOL_H__