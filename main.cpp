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
    caliper_tool.load_data("data" + slash + "data.txt");
    point offset_vector = caliper_tool.calculate_offset_vector();
    cout << "Offset vector: (" << offset_vector.x << ", " << offset_vector.y << ")" << endl;
    return 0;
}
