/*
  ==============================================================================

    FileHandler.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <array>
#include <JuceHeader.h>
#include <vector>

#include "../Globals/Globals.hpp"

class FileHandler
{
public:
    FileHandler();
    ~FileHandler();

    void readAudioFileAndCopyToVector(juce::File& audioFile, std::vector<float>& audioVector);
    void exportFiles(const std::vector<float>& origAudioData,
                     const std::vector<float>& resampledCycles,
                     const std::vector<float>& polarCycles,
                     const std::array<std::vector<float>, N_WT_BANDS>& resynthesizedCycles,
                     const juce::String exportFolderName);

    void saveMonoAudioFile(const std::vector<float> &audioVector,
                           const juce::File& folderOfThisExport,
                           const juce::String fileName);
    
    void saveMultiChannelAudioFile(const std::array<std::vector<float>, N_WT_BANDS>& arrayOfVectors,
                                   const juce::File& folderOfThisExport,
                                   const juce::String fileName);
    
    void saveVectorAsTextFileOnDesktop(std::vector<float>& vector, juce::String fileName);

private:
    void normalizeAudioVector(std::vector<float>& audioData);
    
    juce::AudioFormatManager mFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> mReaderSource;
};
