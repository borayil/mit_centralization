/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: This file contains the implementation of the MIT class.
 * Mathematical visualization of centralization approach:
 * https://www.geogebra.org/calculator/bs8amyqz
 */
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
        clog << "* Pre-calculated cos and sin values for each finger" << endl;
    }
    else
    {
        cerr << "* Unable to open readings file" << endl;
    }
}

point mit::calculate_offset_vector()
{
    point result_offset_vector;
    vector<point> offset_vectors;
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

            // Based on reading_distance, we can decide on the direction of the offset vector
            // If reading_distance is less than pipe_radius, then the offset vector should be in the opposite direction
            // because the finger is closer to the wall rather than the center of the pipe
            if (reading_distance < pipe_radius)
            {
                offset_vector.x *= -1;
                offset_vector.y *= -1;
            }
            average_offset_vector_at_depth.x += offset_vector.x;
            average_offset_vector_at_depth.y += offset_vector.y;
        }
        average_offset_vector_at_depth.x /= no_of_fingers;
        average_offset_vector_at_depth.y /= no_of_fingers;
        offset_vectors.push_back(average_offset_vector_at_depth);

        result_offset_vector.x += average_offset_vector_at_depth.x;
        result_offset_vector.y += average_offset_vector_at_depth.y;
    }

    result_offset_vector.x /= readings.size();
    result_offset_vector.y /= readings.size();

    return result_offset_vector;
}

void mit::centralize_readings(point offset_vector)
{
    // Calculate pipe center estimate
    point pipe_center_estimate;
    pipe_center_estimate.x = -offset_vector.x;
    pipe_center_estimate.y = -offset_vector.y;

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

void mit::save_centralized_readings(string filename)
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

void mit::show_readings(bool show_centralized_readings)
{
    // Format: (depth, finger) = old reading -> new reading
    cout << fixed;
    // Show header

    if (show_centralized_readings)
    {
        cout << "(depth, finger) = old reading -> new reading" << endl;
        for (size_t depth = 0; depth < readings.size(); depth++)
        {
            for (size_t finger = 0; finger < readings[0].size(); finger++)
            {
                cout << "(" << depth << ", " << finger << ") = " << readings[depth][finger].distance << " -> " << readings[depth][finger].centralized_distance << endl;
            }
        }
    }
    else
    {
        cout << "(depth, finger) = old reading" << endl;
        for (size_t depth = 0; depth < readings.size(); depth++)
        {
            for (size_t finger = 0; finger < readings[0].size(); finger++)
            {
                cout << "(" << depth << ", " << finger << ") = " << readings[depth][finger].distance << endl;
            }
        }
    }
}

double mit::calculate_distance(point a, point b)
{
    double dx = b.x - a.x;
    double dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}
