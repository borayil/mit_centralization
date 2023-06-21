/*
 * File: main.cpp
 * Author: Bora Yilmaz
 * Email: borayil00@gmail.com
 * Description: This file contains the implementation of the MIT class.
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
    this->pipe_radius = pipe_diameter_mm / 2.0;
    assert(no_of_fingers % 2 == 0);          // Fingers should have corresponding opposite fingers
    assert(no_of_fingers > 0);               // There should be at least one finger
    assert(pipe_diameter_mm > 0);            // Pipe diameter should be positive
    assert(distance_between_samples_mm > 0); // Distance between samples should be positive

    clog << "* Initialized a new MIT with " << no_of_fingers << " fingers, "
         << pipe_diameter_mm << " mm pipe diameter, "
         << pipe_radius << " mm pipe radius and "
         << distance_between_samples_mm << " mm distance between samples" << endl;
}

void mit::load_readings(const string filename)
{
    if (filename.empty())
        throw invalid_argument("Filename cannot be empty");

    ifstream inputFile(filename);

    if (!inputFile.is_open())
        throw invalid_argument("Unable to open readings file");

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
        assert(finger >= no_of_fingers); // Ensure that we have enough readings
    }
    inputFile.close();
    assert((double)no_of_fingers == readings[0].size());
    clog << "* Loaded readings successfully from " << filename << endl;

    precalculate_cos_sin_values(no_of_fingers, cos_values, sin_values);
}

point mit::calculate_offset_vector_of_sample(const size_t sample)
{
    // Find maximum and minimum readings since they represent the diameter best.

    reading max_reading;
    reading min_reading;
    max_reading.distance = -1;

    for (size_t finger = 0; finger < readings[sample].size(); finger++)
    {

        // Finger reading
        double distance = readings[sample][finger].distance;

        // Opposite finger reading
        int opposite_finger = get_opposite_finger_index(finger, no_of_fingers);
        double distance_opposite = readings[sample][opposite_finger].distance;

        // Compare distance and distance opposite with min and max readings.
        // Update min and max accordingly.
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
    }

    point contact_point{0, 0};
    contact_point.x = min_reading.distance * cos_values[min_reading.finger];
    contact_point.y = min_reading.distance * sin_values[min_reading.finger];
    double min_angle = atan2(contact_point.y, contact_point.x); // angle from min_reading finger
    double xc = ((max_reading.distance - min_reading.distance) / 2) * cos(min_angle);
    double yc = ((max_reading.distance - min_reading.distance) / 2) * sin(min_angle);
    return {xc, yc}; // position of the tool w.r.t. (0,0) (center of the pipe)
}

void mit::centralize_readings(const point offset_vector)
{

    point pipe_center{-offset_vector.x, -offset_vector.y};
    for (size_t depth = 0; depth < readings.size(); depth++)
    {
        for (size_t finger = 0; finger < readings[0].size(); finger++)
        {
            // Create vectors to the contact point of the finger
            point contact_point{0, 0};
            contact_point.x = readings[depth][finger].distance * cos_values[finger];
            contact_point.y = readings[depth][finger].distance * sin_values[finger];

            // Calculate the new distance from the new contact point vector
            readings[depth][finger].distance = calculate_distance(pipe_center, contact_point);
            readings[depth][finger].is_centralized = true;
        }
    }
}

void mit::save_centralized_readings(const string filename)
{
    if (filename.empty())
        throw invalid_argument("Filename cannot be empty");

    ofstream outputFile(filename);
    if (!outputFile.is_open())
        throw invalid_argument("Unable to open readings file");
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