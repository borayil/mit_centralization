#include "mit.h"
using namespace std;

mit::mit(const int no_of_fingers,
         const double pipe_diameter_mm,
         const double distance_between_samples_mm)
{
    if (no_of_fingers < 1)
    {
        cerr << "* Error: Number of fingers ( " << no_of_fingers << " ) must be greater than 0" << endl;
        exit(1);
    }
    else
    {
        this->no_of_fingers = no_of_fingers;
    }

    if (pipe_diameter_mm < 1)
    {
        cerr << "* Error: Pipe diameter ( " << pipe_diameter_mm << " ) must be greater than 0" << endl;
        exit(1);
    }
    else
    {
        this->pipe_diameter = pipe_diameter_mm;
        this->pipe_radius = pipe_diameter_mm / 2.0;
    }

    if (distance_between_samples_mm < 1)
    {
        cerr << "* Error: Distance between samples ( " << distance_between_samples_mm << " ) must be greater than 0" << endl;
        exit(1);
    }
    else
    {
        this->distance_between_samples_mm = distance_between_samples_mm;
    }

    clog << "* Initialized a new MIT with " << no_of_fingers << " fingers, "
         << pipe_diameter_mm << " mm pipe diameter, "
         << pipe_radius << " mm pipe radius and "
         << distance_between_samples_mm << " mm distance between samples" << endl;
}

void mit::load_data(string filename)
{
    ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        string line;
        // Read data line by line
        while (getline(inputFile, line))
        {
            vector<double> finger_readings_at_depth;
            double distance;
            stringstream ss(line);
            while (ss >> distance)
            {

                finger_readings_at_depth.push_back(distance);
            }
            readings.push_back(finger_readings_at_depth);
        }
        inputFile.close();
        if ((double)no_of_fingers != readings[0].size())
        {
            cerr << "* Error: Number of fingers ( " << no_of_fingers << " ) does not match number of readings ( " << readings[0].size() << " )" << endl;
            exit(1);
        }

        clog << "* Loaded readings from " << filename << endl;

        double angle_increment = 2 * M_PI / no_of_fingers;

        for (int finger = 0; finger < no_of_fingers; finger++)
        {
            double angle = finger * angle_increment;
            cos_values.push_back(cos(angle));
            sin_values.push_back(sin(angle));
        }
        cout << "* Pre-calculated cos and sin values for each finger" << endl;
    }
    else
    {
        cerr << "* Unable to open readings file" << endl;
    }
}

point mit::calculate_offset_vector()
{
    /*
    1. Assume we are the origin (0,0). Doesnt matter. We are the tool looking for the center of the pipe.
    2. Calculate expected reading vector for finger i (we have angle for i, and have expected radius)
    3. Calculatae vector to actual reading as well with angle and the distance reading
    4. Get difference expected - actual vector
    5. Get average these difference vectors
    */
    vector<point> difference_vectors_at_depth;
    for (size_t depth = 0; depth < readings.size(); depth++)
    {

        point average_offset_vector_at_depth;
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            double cos_value = cos_values[finger];
            double sin_value = sin_values[finger];
            point expected_reading;
            expected_reading.x = cos_value * pipe_radius;
            expected_reading.y = sin_value * pipe_radius;

            point actual_reading;
            double reading_distance = readings[depth][finger];
            actual_reading.x = cos_value * reading_distance;
            actual_reading.y = sin_value * reading_distance;

            point offset_vector;
            offset_vector.x = expected_reading.x - actual_reading.x;
            offset_vector.y = expected_reading.y - actual_reading.y;

            average_offset_vector_at_depth.x += offset_vector.x;
            average_offset_vector_at_depth.y += offset_vector.y;
        }
        average_offset_vector_at_depth.x /= no_of_fingers;
        average_offset_vector_at_depth.y /= no_of_fingers;
        difference_vectors_at_depth.push_back(average_offset_vector_at_depth);
        // cout << "Depth: " << depth << " Offset vector: (" << average_offset_vector_at_depth.x << ", " << average_offset_vector_at_depth.y << ")" << endl;
    }

    point average_offset_vector;
    for (size_t i = 0; i < difference_vectors_at_depth.size(); i++)
    {
        average_offset_vector.x += difference_vectors_at_depth[i].x;
        average_offset_vector.y += difference_vectors_at_depth[i].y;
    }

    average_offset_vector.x /= difference_vectors_at_depth.size();
    average_offset_vector.y /= difference_vectors_at_depth.size();
    return average_offset_vector;
}

void mit::centralize_readings(point offset_vector)
{
    // Reverse the offset on the tool which we assumed initially to be at (0,0)
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
        }
    }
}
