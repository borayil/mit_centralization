// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <string>

constexpr int NO_OF_FINGERS = 40;
constexpr double PIPE_DIAMETER_MM = 254.0;
constexpr double DISTANCE_BETWEEN_SAMPLES_MM = 5.0;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
const std::string SLASH = "\\";
#else
const std::string SLASH = "/";
#endif

const std::string PATH_TO_DATA = ".." + SLASH + std::string("data") + SLASH + "data.txt";
const std::string PATH_TO_TRANSFORMED_DATA = ".." + SLASH + std::string("data") + SLASH + "transformed_data.txt";

#endif // CONFIG_H
