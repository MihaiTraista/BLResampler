#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"
#include "../Fourier/Fourier.hpp"

class Playback{
public:
    Playback(const std::vector<float>& av, int start, int length);
    
    void readSamplesFromVector(const juce::AudioSourceChannelInfo& bufferToFill);
    inline void setAudioVector(const std::vector<float>& av, int start, int length){
        pAudioVector = &av;
        mReadingStart = start;
        mReadingLength = length;
        mSampleCounter = 0;
    };

    inline void setReadingStart(int val){ mReadingStart = val; };
    inline void setReadingLength(int val){ mReadingLength = val; };

private:
    int mSampleCounter = 0;
    const std::vector<float>* pAudioVector = nullptr;
    int mReadingStart = 0;
    int mReadingLength = 0;
};
