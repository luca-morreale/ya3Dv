
#include "utils.hpp"


std::string& ltrim(std::string& str, const std::string chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string chars)
{
    return ltrim(rtrim(str, chars), chars);
}


std::array<float, 3> HSVtoRGB(int H, double S, double V)
{
    double C = S * V;
    double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
    double m = V - C;
    double Rs, Gs, Bs;

    if(H >= 0 && H < 60) {
        Rs = C;
        Gs = X;
        Bs = 0;
    }
    else if(H >= 60 && H < 120) {
        Rs = X;
        Gs = C;
        Bs = 0;
    }
    else if(H >= 120 && H < 180) {
        Rs = 0;
        Gs = C;
        Bs = X;
    }
    else if(H >= 180 && H < 240) {
        Rs = 0;
        Gs = X;
        Bs = C;
    }
    else if(H >= 240 && H < 300) {
        Rs = X;
        Gs = 0;
        Bs = C;
    }
    else {
        Rs = C;
        Gs = 0;
        Bs = X;
    }

    std::array<float, 3> output;
    output[0] = (Rs + m) * 255;
    output[1] = (Gs + m) * 255;
    output[2] = (Bs + m) * 255;
    return output;
}
