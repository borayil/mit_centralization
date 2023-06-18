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
    mit caliper_tool(40, 254, 5);
    caliper_tool.load_readings("data" + slash + "data.txt");
    point offset_vector = caliper_tool.calculate_offset_vector();
    // point offset_vector = caliper_tool.calculate_offset_vector();
    caliper_tool.centralize_readings(offset_vector);
    caliper_tool.save_readings("data" + slash + "data_centralized.txt");

    return 0;
}
