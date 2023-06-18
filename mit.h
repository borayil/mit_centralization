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
    double centralized_distance;
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
    // std::vector<point> contact_points;          // Contact points w.r.t. the tool
public:
    mit(const int no_of_fingers,
        const double pipe_diameter_mm,
        const double distance_between_samples_mm);
    void load_readings(const std::string filename);
    void save_readings(const std::string filename);
    point calculate_offset_vector();
    void calculate_contact_points();
    void centralize_readings(const point offset_vector);
    double calculate_distance(const point a, const point b);
};

#endif // __CALIPER_TOOL_H__