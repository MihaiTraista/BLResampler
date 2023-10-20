/*
  ==============================================================================

    ZeroCrossingFinder.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <vector>

#include <JuceHeader.h>

class ZeroCrossingFinder
{
public:
    ZeroCrossingFinder();
    ~ZeroCrossingFinder();

    void calculateZeroCrossings(const juce::AudioBuffer<float>& mAudioFileBuffer,
                                std::vector<bool>& mZeroCrossings);
    void findClosestZeroCrossingsToCycleLenHint(const std::vector<bool>& mZeroCrossings,
                                                int& mClosestZeroCrossingStart,
                                                int& mClosestZeroCrossingEnd,
                                                const int& mStartSampleIndex,
                                                const int& mCycleLenHint);

private:    
};
