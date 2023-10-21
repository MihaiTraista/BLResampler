/*
  ==============================================================================

    Resampler.cpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#include "Resampler.hpp"

Resampler::Resampler(){}

Resampler::~Resampler(){}

void Resampler::resizeCycle(const std::vector<float>& origCycle, std::vector<float>& resizedWaveform){
    int origLength = static_cast<int>(origCycle.size());
    
    if(origLength == WTSIZE) {
        // No resize needed
        resizedWaveform = origCycle;
    }
    else if(origLength < WTSIZE) {
        for(int i = 0; i < WTSIZE; ++i){
            float position = static_cast<float>(i) * origLength / static_cast<float>(WTSIZE);
            int indexInOrig = static_cast<int>(position);
            float frac = position - indexInOrig;

            if(mUseCubicInterpolation && indexInOrig > 0 && indexInOrig < origLength - 2) {
                float sample = cubicInterpolate(
                    origCycle[indexInOrig - 1],
                    origCycle[indexInOrig],
                    origCycle[indexInOrig + 1],
                    origCycle[indexInOrig + 2],
                    frac);
                resizedWaveform[i] = sample;
            } else {
                // Linear interpolation
                float sample = (1.0f - frac) * origCycle[indexInOrig] + frac * origCycle[(indexInOrig + 1) % origLength];
                resizedWaveform[i] = sample;
            }
        }
    }
    else {
        for(int i = 0; i < WTSIZE; ++i){
            int indexInOrig = i * origLength / static_cast<float>(WTSIZE);
            resizedWaveform[i] = origCycle[indexInOrig];
        }
    }
}

float Resampler::cubicInterpolate(float y0,float y1, float y2,float y3, float mu){
    float a0, a1, a2, a3, mu2;

    mu2 = mu*mu;
    a0 = y3 - y2 - y0 + y1;
    a1 = y0 - y1 - a0;
    a2 = y2 - y0;
    a3 = y1;

    return(a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}
