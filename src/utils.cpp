#include "utils.h"
using namespace std;

void show_readings(vector<vector<reading>> readings)
{
    cout << fixed << "(depth, finger) = old reading" << endl;
    for (size_t sample = 0; sample < readings.size(); sample++)
    {
        cout << "Depth " << sample;
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            cout << readings[sample][finger].distance << endl;
        }
        cout << "---------------------" << endl;
    }
}

double calculate_distance(const point a, const point b)
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

double calculate_vector_length(const point a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

size_t get_opposite_finger_index(const size_t finger, const size_t no_of_fingers)
{
    return (finger + no_of_fingers / 2) % no_of_fingers;
}

void precalculate_cos_sin_values(const size_t no_of_fingers,
                                 vector<double> &cos_values,
                                 vector<double> &sin_values)
{
    for (size_t finger = 0; finger < no_of_fingers; finger++)
    {
        double angle = (finger * 360.0) / no_of_fingers;
        double radian = angle * M_PI / 180.0;
        cos_values.push_back(cos(radian));
        sin_values.push_back(sin(radian));
    }
}