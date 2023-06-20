#include "utils.h"
using namespace std;

double calculate_distance(point a, point b)
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

double calculate_vector_length(point a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}