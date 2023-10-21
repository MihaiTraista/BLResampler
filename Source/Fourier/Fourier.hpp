#pragma once

#include <algorithm>
#include <vector>

#include "../Globals/Globals.hpp"

class Fourier{
public:
    inline Fourier() = delete; // we delete the constructor because this is a utility class
    
    static void fillDftPolar(const std::vector<float>& rawWavetable, std::vector<float>& polarValues);

    static void idft(const std::vector<float>& polarValues,
                     std::vector<float>& resynthesized,
                     int harmonicLimit,
                     float rollOffPercent,
                     int nCycles);
    
    static void removeDCOffset(int bandIndex, std::vector<std::vector<float>>& blw);
    
    static void rotateWavetableToNearestZero(std::vector<float>& wt);
    
    static void applyLinearSlant(std::vector<float>& waveform, int nCycles);

private:
};
