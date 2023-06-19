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

// TODO, we are not updating contact point for all depths
// So do that in another function
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

        point offset_vector{0, 0};
        offset_vector.x = expected_contact_point.x - actual_contact_point.x;
        offset_vector.y = expected_contact_point.y - actual_contact_point.y;

        result_offset_vector.x += offset_vector.x;
        result_offset_vector.y += offset_vector.y;
    }
    result_offset_vector.x /= no_of_fingers;
    result_offset_vector.y /= no_of_fingers;

    return result_offset_vector;
}

void mit::calculate_contact_points()
{
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            double cos_value = cos_values[finger];
            double sin_value = sin_values[finger];
            point contact_point{0, 0};
            contact_point.x = cos_value * readings[depth][finger].distance;
            contact_point.y = sin_value * readings[depth][finger].distance;
            readings[depth][finger].contact_point = contact_point;
        }
    }
}

void mit::centralize_readings(const point offset_vector)
{
    point pipe_center_estimate{-offset_vector.x, -offset_vector.y};
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            // Original reading contact point w.r.t. tool at (0, 0)
            point contact_point = readings[depth][finger].contact_point;

            // Vector from pipe center estimate to its expected contact point
            point v_expected_contact_point{0, 0};
            v_expected_contact_point.x = pipe_center_estimate.x + cos_values[finger] * pipe_radius;
            v_expected_contact_point.y = pipe_center_estimate.y + sin_values[finger] * pipe_radius;

            // Vector from pipe center estimate to the original contact point
            point v_original_contact_point{0, 0};
            v_original_contact_point.x = contact_point.x - pipe_center_estimate.x;
            v_original_contact_point.y = contact_point.y - pipe_center_estimate.y;

            // Calculate the angle between the two vectors using dot product
            double dot_product = v_expected_contact_point.x * v_original_contact_point.x + v_expected_contact_point.y * v_original_contact_point.y;
            double angle = acos(dot_product / (calculate_distance({0, 0}, v_expected_contact_point) * calculate_distance({0, 0}, v_original_contact_point)));

            // Translate the point we want to rotate to the origin
            point v_original_contact_point_translated{0, 0};
            v_original_contact_point_translated.x = v_original_contact_point.x - pipe_center_estimate.x;
            v_original_contact_point_translated.y = v_original_contact_point.y - pipe_center_estimate.y;

            // Rotate the point
            point v_original_contact_point_rotated{0, 0};
            v_original_contact_point_rotated.x = v_original_contact_point_translated.x * cos(angle) - v_original_contact_point_translated.y * sin(angle);
            v_original_contact_point_rotated.y = v_original_contact_point_translated.x * sin(angle) + v_original_contact_point_translated.y * cos(angle);

            // Without translating back, we cna calculate from the origin
            // It is the same distance.
            double distance = calculate_distance({0, 0}, v_original_contact_point_rotated);
            readings[depth][finger].centralized_distance = distance;
            readings[depth][finger].is_centralized = true;
        }
    }
}

void mit::centralize_readings_2(const point offset_vector)
{
    point pipe_center_estimate{-offset_vector.x, -offset_vector.y};
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            // Original reading contact point w.r.t. tool at (0, 0)
            point contact_point = readings[depth][finger].contact_point;

            // Vector from pipe center estimate to its expected contact point
            point v_expected_contact_point{0, 0};
            v_expected_contact_point.x = pipe_center_estimate.x + cos_values[finger] * pipe_radius;
            v_expected_contact_point.y = pipe_center_estimate.y + sin_values[finger] * pipe_radius;

            // Move contact_point to expected contact point
            point v_contact_to_expected{0, 0};
            v_contact_to_expected.x = v_expected_contact_point.x - contact_point.x;
            v_contact_to_expected.y = v_expected_contact_point.y - contact_point.y;

            // Add vector to contact point
            point v_new_contact_point{0, 0};
            v_new_contact_point.x = contact_point.x + v_contact_to_expected.x;
            v_new_contact_point.y = contact_point.y + v_contact_to_expected.y;

            // Calculate distance
            double distance = calculate_distance(pipe_center_estimate, v_new_contact_point);
            readings[depth][finger].centralized_distance = distance;
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
