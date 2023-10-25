#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"
#include "../Fourier/Fourier.hpp"

class Playback{
public:
    Playback(const std::vector<float>* pAV);
    
    void readSamplesFromVector(const juce::AudioSourceChannelInfo& bufferToFill);
    inline void setAudioVector(const std::vector<float>* pAV){
        pAudioVector = pAV;
    };
    
private:
    int mSampleCounter = 0;
    const std::vector<float>* pAudioVector = nullptr;
};
