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
            int finger = 0;
            while (ss >> distance)
            {
                r.finger = finger;
                finger++;
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
    point resultant_offset_vector{0, 0};
    reading min_reading;
    reading max_reading;
    min_reading.distance = 999999;
    max_reading.distance = -999999;
    for (size_t finger = 0; finger < readings[depth].size(); finger++)
    {

        // Finger readings
        double distance = readings[depth][finger].distance;
        double cos_value = cos_values[finger];
        double sin_value = sin_values[finger];

        // Opposite finger readings
        int opposite_finger = (finger + no_of_fingers / 2) % no_of_fingers;
        double distance_opposite = readings[depth][opposite_finger].distance;
        double opposite_cos_value = cos_values[opposite_finger];
        double opposite_sin_value = sin_values[opposite_finger];

        // Compare distance and distance opposite with min and max readings.
        // Update min and max accordingly. After this loop, we will have the Lmax and Lmin
        // values for given dpth.
        if (distance > max_reading.distance)
        {
            max_reading.distance = distance;
            max_reading.finger = finger;
            min_reading.distance = distance_opposite;
            min_reading.finger = opposite_finger;
            continue;
        }
        else if (distance_opposite > max_reading.distance)
        {
            max_reading.distance = distance_opposite;
            max_reading.finger = opposite_finger;
            min_reading.distance = distance;
            min_reading.finger = finger;
            continue;
        }

        if (distance < min_reading.distance)
        {
            min_reading.distance = distance;
            min_reading.finger = finger;
            max_reading.distance = distance_opposite;
            max_reading.finger = opposite_finger;
            continue;
        }
        else if (distance_opposite < min_reading.distance)
        {
            min_reading.distance = distance_opposite;
            min_reading.finger = opposite_finger;
            max_reading.distance = distance;
            max_reading.finger = finger;
            continue;
        }

        // We have our max and min readings now. Lets get the angle between contact point of this reading and axis using atan2
        point contact_point{0, 0};
        contact_point.x = min_reading.distance * cos_values[min_reading.finger];
        contact_point.y = min_reading.distance * sin_values[min_reading.finger];
        double min_angle = atan2(contact_point.y, contact_point.x);
        double xc = ((max_reading.distance - min_reading.distance) / 2) * cos(min_angle);
        double yc = ((max_reading.distance - min_reading.distance) / 2) * sin(min_angle);
        // (xc,yc) is the position of the tool center at this depth at its offset.
        return {xc, yc};
    }
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
            // Create vectors to the contact point of the finger
            point v_contact_point{0, 0};
            v_contact_point.x = readings[depth][finger].distance * cos_values[finger];
            v_contact_point.y = readings[depth][finger].distance * sin_values[finger];

            // Add offset vector to contact point vector to get the new contact point vector
            point new_contact_point{0, 0};
            new_contact_point.x = pipe_center.x + v_contact_point.x + offset_vector.x;
            new_contact_point.y = pipe_center.y + v_contact_point.y + offset_vector.y;

            // Calculate the new distance from the new contact point vector
            readings[depth][finger].distance = calculate_distance(pipe_center, new_contact_point);
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
                outputFile << readings[depth][finger].distance << " ";
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

void mit::show_readings()
{
    // Format: (depth, finger) = old reading -> new reading
    cout << fixed;
    // Show header

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