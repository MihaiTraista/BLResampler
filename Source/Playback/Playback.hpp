#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"

class Playback{
public:
    Playback(std::vector<float>& resampledCycles,
             std::vector<float>& polarCycles,
             std::vector<float>& resynthesizedCycles);
    
    void audioCallback(const juce::AudioSourceChannelInfo& bufferToFill, std::vector<float>& vectorToReadFrom);
    
private:
    int mSampleCounter = 0;
    
    std::vector<float>& mResampledCycles;
    std::vector<float>& mPolarCycles;
    std::vector<float>& mResynthesizedCycles;
};
