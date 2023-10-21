#include <iostream>
#include <cassert>
#include <cmath>
#include <chrono>

#include "Fourier.hpp"


void Fourier::fillDftPolar(const float* rawWavetableData, float* polarValues){
    float realImag[WTSIZE * 2], real, imag;

    for(int i = 0, k = 0; k < WTSIZE; i += 2, k++){
        realImag[i] = realImag[i + 1] = 0.0f;
        for(int n = 0; n < WTSIZE; n++){
            realImag[i] += static_cast<float>(rawWavetableData[n] * std::cos((k * n * TWOPI)/WTSIZE));
            realImag[i + 1] -= static_cast<float>(rawWavetableData[n] * std::sin((k * n * TWOPI)/WTSIZE));
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

void Fourier::idft(float* polarValues, std::vector<float>& resynthesized){
    float amp, phase;

    for (int i = 0; i < WTSIZE; i++) {     //  number of samples = WTSIZE
        
        resynthesized[i] = 0;
        
        for (int h = 0; h < WTSIZE; h++) {   //  number of harmonics = WTSIZE
            amp = polarValues[h * 2];
            phase = polarValues[h * 2 + 1];
//            phase = (static_cast<float>(i) / static_cast<float>(WTSIZE)) * TWOPI - M_PI;
            phase = 0.0f;
//            amp = 1.0f;

            // from the frequency-domain representation, we can construct the time-domain signal in two ways.

            // 1. reconstruct from the real and imaginary values
            // this is applying the inverse Discrete Fourier Transform (iDFT) formula directly.
            // real corresponds to the magnitude of the cosine component of the signal, and imag corresponds to the magnitude of the sine component.
            // float val = real * cos(TWO_PI * i * h / WTSIZE) - imag * sin(TWO_PI * i * h / WTSIZE);

            // 2. reconstruct from the polar coordinates instead of rectangular coordinates.
            float val = amp * cos(TWOPI * i * h / WTSIZE + phase);

            resynthesized[i] += val;
        }
    }
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
