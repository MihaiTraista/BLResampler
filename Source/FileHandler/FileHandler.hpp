/*
  ==============================================================================

    FileHandler.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    void saveResampledFileOnDisk(std::vector<float>& mResampledCycles);
    void storeAudioFileInBuffer(juce::File& audioFile, juce::AudioBuffer<float>& mAudioFileBuffer);
    
private:    
    juce::AudioFormatManager mFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> mReaderSource;
};
