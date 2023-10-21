#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>

#include "Fourier.hpp"


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
                
//                phase = -M_PI;

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
//    float f1 = tempResynthesized[0];
//    float f2 = tempResynthesized[1];
//    float l1 = tempResynthesized[1022];
//    float l2 = tempResynthesized[1023];
//    l2 = 0.0f;

    resynthesized = tempResynthesized;
}

void Fourier::removeDCOffset(int bandIndex, std::vector<std::vector<float>>& blw){
//    float firstVal = blw[bandIndex][cycleIndex * WTSIZE];
//    float lastVal = blw[bandIndex][cycleIndex * WTSIZE + WTSIZE - 1];
//    float midpoint = (firstVal + lastVal) / 2.0f;
//    float offset = midpoint * -1.0f;
//    for (int i = cycleIndex * WTSIZE; i < (cycleIndex + 1) * WTSIZE; i++){
//        blw[bandIndex][i] += offset;
//        assert(fabs(blw[bandIndex][i]) <= 1.0f && "Sample values can't exceed an amplitude of 1.0f");
//    }
}

void Fourier::rotateWavetableToNearestZero(std::vector<float>& wt){
    int dephaseIndex = 0;
    float minAbsValue = std::fabs(wt[0]);
    
    // Find the index of the value closest to zero
    for(int i = 1; i < WTSIZE; ++i){
        float absValue = std::fabs(wt[i]);
        if(absValue < minAbsValue){
            minAbsValue = absValue;
            dephaseIndex = i;
        }
    }

    std::vector<float> rotatedWt(WTSIZE);

    for(int i = 0; i < WTSIZE; ++i){
        rotatedWt[i] = wt[(i + dephaseIndex) % WTSIZE];
    }

    wt = rotatedWt;
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
