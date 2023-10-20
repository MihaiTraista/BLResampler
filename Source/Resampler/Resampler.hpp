/*
  ==============================================================================

    Resampler.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <vector>

#include <JuceHeader.h>

class Resampler
{
public:
    Resampler();
    ~Resampler();
    
    void resample(const std::vector<float>& origCycle, std::vector<float>& mResampledCycles);
    inline void setLengthOfResampledCycle(int newValue) { mResampledCycleLength = newValue; };
    inline int getLengthOfResampledCycle() { return mResampledCycleLength; };
    
private:
    float cubicInterpolate(float y0,float y1, float y2,float y3, float mu);
    bool mUseCubicInterpolation = true;
    int mResampledCycleLength = 1024;
};
