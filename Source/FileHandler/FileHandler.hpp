/*
  ==============================================================================

    FileHandler.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    void readAudioFileAndCopyToVector(juce::File& audioFile, std::vector<float>& audioVector);
    void saveVectorAsAudioFileToDesktop(const std::vector<float>& audioData, const juce::String fileName);
    
private:    
    juce::AudioFormatManager mFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> mReaderSource;
};
