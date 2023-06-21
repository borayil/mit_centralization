#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include "mit.h"

// Forward declarations
struct point;
struct reading;

void show_readings(std::vector<std::vector<reading>> readings);
double calculate_distance(point a, point b);
double calculate_vector_length(point a);
size_t get_opposite_finger_index(size_t finger, size_t no_of_fingers);
void precalculate_cos_sin_values(
    size_t no_of_fingers,
    std::vector<double> &cos_values,
    std::vector<double> &sin_values); // Pre-calculation to be efficient, accesible by finger index

#endif
