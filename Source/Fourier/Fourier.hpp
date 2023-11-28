#pragma once

#include <algorithm>
#include <array>
#include <vector>
#include <complex>

#include "../Globals/Globals.hpp"
#include "../JuriHock/FFT.h"
#include "../JuriHock/RFFT.h"

class Fourier{
public:
    inline Fourier() = delete; // we delete the constructor because this is a utility class
    
    static void execute(const std::vector<float>& resampledCycle,
                        std::vector<float>& mPolarCycles,
                        std::array<std::vector<float>, N_WT_BANDS>& mResynthesizedCycles);

private:
    
    struct Cache {
        std::vector<float> polar;
        std::vector<std::complex<float>> rect;
        std::vector<float> polarTempBL;
        std::vector<std::complex<float>> rectTempBL;
        std::array<std::vector<float>, N_WT_BANDS> resynthesized;

        Cache():
            polar(WTSIZE),
            rect(WTSIZE / 2 + 1),
            polarTempBL(WTSIZE),
            rectTempBL(WTSIZE / 2 + 1)
        {
            for (auto& vec : resynthesized)
                vec.resize(WTSIZE, 0.0f);
        }
    };

    static void calculateCacheRect(const std::vector<float>& resampledCycle);
    static void calculateCachePolar();
    static void calculateCacheBandLimited();
        
    static void idftBand(std::vector<float>& resynthesized,
                         int harmonicLimit,
                         float rollOffPercent);
    
    static void convertCartesianToPolar(std::vector<std::complex<float>>& dft, std::vector<float>& polar);
    static void convertPolarToCartesian(std::vector<float>& polar, std::vector<std::complex<float>>& dft);
    static void applyLinearSlant(std::vector<float>& waveform, int nCycles);
    static void appendCachesToVectors(std::vector<float>& mPolarCycles,
                                      std::array<std::vector<float>, N_WT_BANDS>& mResynthesizedCycles);
    static void clearCacheBandLimited();

    static stftpitchshift::RFFT smJuriHock;
    static Cache smCache;
};
