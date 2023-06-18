#include "mit.h"
using namespace std;

mit::mit(const int no_of_fingers,
         const double pipe_diameter_mm,
         const double distance_between_samples_mm)
    : no_of_fingers(no_of_fingers), pipe_diameter(pipe_diameter_mm),
      pipe_radius(pipe_diameter_mm / 2.0),
      distance_between_samples_mm(distance_between_samples_mm)
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

void mit::load_readings(string filename)
{
    ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        string line;
        // Read data line by line
        while (getline(inputFile, line))
        {
            vector<reading> finger_readings_at_depth;
            reading r;
            double distance;
            stringstream ss(line);
            while (ss >> distance)
            {

                r.distance = distance;
                finger_readings_at_depth.push_back(r);
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
    point result_offset_vector;
    for (size_t depth = 0; depth < readings.size(); depth++)
    {

        point average_offset_vector_at_depth;
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            double cos_value = cos_values[finger];
            double sin_value = sin_values[finger];
            point expected_contact_point;
            expected_contact_point.x = cos_value * pipe_radius;
            expected_contact_point.y = sin_value * pipe_radius;

            point actual_contact_point;
            double reading_distance = readings[depth][finger].distance;
            actual_contact_point.x = cos_value * reading_distance;
            actual_contact_point.y = sin_value * reading_distance;
            readings[depth][finger].contact_point = actual_contact_point; // Store the contact point w.r.t. the tool
            // Print out actual reading vs expected reading
            point offset_vector;
            offset_vector.x = expected_contact_point.x - actual_contact_point.x;
            offset_vector.y = expected_contact_point.y - actual_contact_point.y;

            average_offset_vector_at_depth.x += offset_vector.x;
            average_offset_vector_at_depth.y += offset_vector.y;
        }

        result_offset_vector.x += average_offset_vector_at_depth.x / no_of_fingers;
        result_offset_vector.y += average_offset_vector_at_depth.y / no_of_fingers;
    }

    result_offset_vector.x /= readings.size();
    result_offset_vector.y /= readings.size();
    clog << "* Offset vector: (" << result_offset_vector.x << ", " << result_offset_vector.y << ")" << endl;
    return result_offset_vector;
}

void mit::centralize_readings(point offset_vector)
{
    // Reverse the offset on the tool which we assumed initially to be at (0,0)
    point pipe_center_estimate;
    pipe_center_estimate.x = -offset_vector.x;
    pipe_center_estimate.y = -offset_vector.y;

    clog << "* Pipe center estimate: (" << pipe_center_estimate.x << ", " << pipe_center_estimate.y << ")" << endl;

    // Centralize readings
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            point contact_point = readings[depth][finger].contact_point;
            readings[depth][finger].centralized_distance = calculate_distance(pipe_center_estimate, contact_point);
        }
    }
}

void mit::save_readings(string filename)
{
    ofstream outputFile(filename);
    if (outputFile.is_open())
    {
        for (size_t depth = 0; depth < readings.size(); depth++)
        {
            for (size_t finger = 0; finger < readings[0].size(); finger++)
            {
                outputFile << readings[depth][finger].centralized_distance << " ";
            }
            outputFile << endl;
        }
        outputFile.close();
        clog << "* Saved readings to " << filename << endl;
    }
    else
    {
        cerr << "* Unable to open readings file" << endl;
    }
}

double mit::calculate_distance(point a, point b)
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}
