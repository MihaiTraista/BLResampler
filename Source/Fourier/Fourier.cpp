#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>

#include "Fourier.hpp"


void Fourier::fill(const std::vector<float>& resampledCycle,
                   std::vector<float>& polar,
                   std::array<std::vector<float>, N_WT_BANDS>& resynthesized){

    std::array<std::vector<float>, N_WT_BANDS> resynthesizedWorker;
    for(auto& vec : resynthesizedWorker)
        vec = std::vector<float>(WTSIZE, 0.0f);

    Fourier::fillDftPolar(resampledCycle, polar);
    
    for(int band = 0; band < N_WT_BANDS; band++){
        float harmonicsLimit = 22050.0f / baseFrequencies[band];
        float rollOffPercent = 50.0f;

        Fourier::idft(polar, resynthesizedWorker[band], harmonicsLimit, rollOffPercent, 1);
    }
    
//    isWorkerThreadBusy.store(false);
    std::swap(resynthesized, resynthesizedWorker);
}


void Fourier::fillDftPolar(const std::vector<float>& rawWavetable, std::vector<float>& polarValues){
    float realImag[WTSIZE * 2], real, imag;

    for(int i = 0, k = 0; k < WTSIZE; i += 2, k++){
        realImag[i] = realImag[i + 1] = 0.0f;
        for(int n = 0; n < WTSIZE; n++){
            realImag[i] += static_cast<float>(rawWavetable[n] * std::cos((k * n * TWOPI)/WTSIZE));
            realImag[i + 1] -= static_cast<float>(rawWavetable[n] * std::sin((k * n * TWOPI)/WTSIZE));
        }
        realImag[i] /= WTSIZE;
        realImag[i+1] /= WTSIZE;
    }

    // convert from rectangular (real and imaginary) to polar (amp and phase)
    for (int i = 0; i < WTSIZE; i++){
        real = realImag[i * 2];
        imag = realImag[i * 2 + 1];
        polarValues[i * 2] = sqrt(pow(real, 2) + pow(imag, 2));
        polarValues[i * 2 + 1] = atan2(imag, real);
    }
}

void Fourier::idft(const std::vector<float>& polarValues,
                   std::vector<float>& resynthesized,
                   int harmonicLimit,
                   float rollOffPercent,
                   int nCycles){
    
    std::vector<float> tempResynthesized = std::vector<float>(polarValues.size() / 2, 0.0f);
    resynthesized.resize(polarValues.size() / 2, 0.0f);
    
    float amp, phase, roll;
    float startRolloff = harmonicLimit - harmonicLimit * (rollOffPercent / 100.0f);
//    startRolloff = 200;

    for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
        
        for (int i = 0; i < WTSIZE; i++) {     //  number of samples = WTSIZE
            
            tempResynthesized[i + cycleIndex * WTSIZE] = 0.0f;
            
            for (int h = 0; h < WTSIZE; h++) {   //  number of harmonics = WTSIZE
                amp = polarValues[(h + cycleIndex * WTSIZE) * 2];
                phase = polarValues[(h + cycleIndex * WTSIZE) * 2 + 1];

                // roll off harmonics from startRolloff to harmonicLimit and zero out all higher harmonics
                if (h < startRolloff){
                    amp *= 1.0;
                } else if (h >= startRolloff && h < harmonicLimit){
                    roll = 1 - ((float)h - startRolloff) / (harmonicLimit - startRolloff);
                    roll *= roll; // square roll for exponential roll off
                    amp *= roll;
                } else {
                    amp *= 0.0;
                }

                float val = amp * cos(TWOPI * (i + cycleIndex * WTSIZE) * h / WTSIZE + phase);

                tempResynthesized[i + cycleIndex * WTSIZE] += val;
            }
        }
    }
    
    applyLinearSlant(tempResynthesized, nCycles);

    resynthesized = tempResynthesized;
}

void Fourier::applyLinearSlant(std::vector<float>& waveform, int nCycles) {
    for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
        float startValue = waveform[cycleIndex * WTSIZE];
        float endValue = waveform[(cycleIndex + 1) * WTSIZE - 1];
        float delta = endValue - startValue;

        for (int i = 0; i < WTSIZE; ++i) {
            float ramp = i / static_cast<float>(WTSIZE - 1);
            waveform[cycleIndex * WTSIZE + i] -= (startValue + delta * ramp);
        }
    }
}
