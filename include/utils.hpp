#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <array>
#include <cmath>
#include <string>


std::string& ltrim(std::string& str, const std::string chars = "\t\n\v\f\r ");
std::string& rtrim(std::string& str, const std::string chars = "\t\n\v\f\r ");
std::string& trim(std::string& str, const std::string chars = "\t\n\v\f\r ");

std::array<float, 3> HSVtoRGB(int H, double S, double V);

#endif // UTILS_HPP_
