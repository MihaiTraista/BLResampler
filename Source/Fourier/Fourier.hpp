#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include "../Globals/Globals.hpp"

class Fourier{
public:
    inline Fourier() = delete; // we delete the constructor because this is a utility class
    
    static void fill(const std::vector<float>& resampledCycle,
                     std::vector<float>& polar,
                     std::array<std::vector<float>, N_WT_BANDS>& resynthesized);

    static void fillDftPolar(const std::vector<float>& rawWavetable, std::vector<float>& polarValues);

    static void idft(const std::vector<float>& polarValues,
                     std::vector<float>& resynthesized,
                     int harmonicLimit,
                     float rollOffPercent,
                     int nCycles);
    
    static void applyLinearSlant(std::vector<float>& waveform, int nCycles);

private:
};
