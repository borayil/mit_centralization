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
        double angle = 0;
        for (int finger = 0; finger < no_of_fingers; finger++)
        {
            angle = finger * angle_increment;
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

point mit::calculate_offset_vector_of_sample(const int depth)
{
    point result_offset_vector{0, 0};
    for (size_t finger = 0; finger < readings[depth].size(); finger++)
    {
        double cos_value = cos_values[finger];
        double sin_value = sin_values[finger];
        point expected_contact_point{0, 0};
        expected_contact_point.x = cos_value * pipe_radius;
        expected_contact_point.y = sin_value * pipe_radius;

        point actual_contact_point{0, 0};
        double reading_distance = readings[depth][finger].distance;
        actual_contact_point.x = cos_value * reading_distance;
        actual_contact_point.y = sin_value * reading_distance;

        point diff_vector{0, 0};
        diff_vector.x = expected_contact_point.x - actual_contact_point.x;
        diff_vector.y = expected_contact_point.y - actual_contact_point.y;

        result_offset_vector.x += diff_vector.x;
        result_offset_vector.y += diff_vector.y;
    }
    result_offset_vector.x /= no_of_fingers;
    result_offset_vector.y /= no_of_fingers;

    return result_offset_vector;
}

void mit::centralize_readings(const point offset_vector)
{
    // (Note: we had assumed that our pipe was centered at 0,0)
    // In other words, our tool started at 0,0 and we move it to the center of the pipe now
    point pipe_center{-offset_vector.x, -offset_vector.y};
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {

            // Expected contact point w.r.t. pipe center
            point expected_contact_point_from_center{0, 0};
            expected_contact_point_from_center.x = pipe_center.x + cos_values[finger] * pipe_radius;
            expected_contact_point_from_center.y = pipe_center.y + sin_values[finger] * pipe_radius;

            // Translate so that it is aligned with pipe center
            expected_contact_point_from_center.x -= offset_vector.x;
            expected_contact_point_from_center.y -= offset_vector.y;

            // Finger distance correction
            double reading_distance_from_tool_center = readings[depth][finger].distance;
            double expected_distance_from_pipe_center = calculate_distance(expected_contact_point_from_center, pipe_center);
            double difference = reading_distance_from_tool_center - expected_distance_from_pipe_center;

            // Update reading
            readings[depth][finger].centralized_distance = expected_distance_from_pipe_center - difference;
            readings[depth][finger].is_centralized = true;
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

void mit::show_readings(const bool show_centralized_readings)
{
    // Format: (depth, finger) = old reading -> new reading
    cout << fixed;
    // Show header

    if (show_centralized_readings)
    {
        for (size_t depth = 0; depth < readings.size(); depth++)
        {
            cout << "Depth / Distance from first sample: " << depth * distance_between_samples_mm << " mm" << endl;
            for (size_t finger = 0; finger < readings[0].size(); finger++)
            {
                cout << readings[depth][finger].distance << " -> " << readings[depth][finger].centralized_distance << endl;
            }
            cout << "---------------------" << endl;
        }
    }
    else
    {
        cout << "(depth, finger) = old reading" << endl;
        for (size_t depth = 0; depth < readings.size(); depth++)
        {
            cout << "Depth / Distance from first sample: " << depth * distance_between_samples_mm << " mm" << endl;
            for (size_t finger = 0; finger < readings[0].size(); finger++)
            {
                cout << readings[depth][finger].distance << endl;
            }
            cout << "---------------------" << endl;
        }
    }
}