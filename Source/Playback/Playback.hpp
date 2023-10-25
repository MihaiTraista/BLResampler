#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"
#include "../Fourier/Fourier.hpp"

class Playback{
public:
    Playback();
    
    void readSamplesFromVector(const juce::AudioSourceChannelInfo& bufferToFill, const std::vector<float>& vectorToReadFrom);
    
private:
    int mSampleCounter = 0;
    std::vector<float> mLiveResynthesized = std::vector<float>(WTSIZE, 0.0f);
};
