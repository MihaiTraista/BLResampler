/*
  ==============================================================================

    Resampler.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"

class Resampler
{
public:
    Resampler();
    ~Resampler();
    
    void resizeCycle(const std::vector<float>& origCycle, std::vector<float>& resizedWaveform);
    
private:
    float cubicInterpolate(float y0,float y1, float y2,float y3, float mu);
    bool mUseCubicInterpolation = true;
};
