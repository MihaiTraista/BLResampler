/*
  ==============================================================================

    DataModel.hpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>
#include <vector>

#include "../Globals/Globals.hpp"
#include "../EventInterface/EventInterface.hpp"
#include "../FileHandler/FileHandler.hpp"
#include "../Resampler/Resampler.hpp"
#include "../ZeroCrossingFinder/ZeroCrossingFinder.hpp"

class DataModel
{
public:
    DataModel();
    ~DataModel();
        
    inline int getStartSampleIndex() const { return mStartSampleIndex; };
    inline void setStartSampleIndex(int value) { mStartSampleIndex = value; };
    inline const std::vector<float>& getReferenceForOrigAudioDataVector() { return mOrigAudioData; };
    inline const std::vector<float>& getReferenceForResampledCyclesVector() { return mResampledCycles; };
    inline const std::vector<bool>& getReferenceForZeroCrossingsVector() { return mZeroCrossings; };
    inline const std::vector<bool>& getReferenceForVectorThatShowsWhichSamplesAreCommitted() { return mVectorThatShowsWhichSamplesAreCommitted; };
    inline const int& getReferenceOfClosestZeroCrossingStart() { return mClosestZeroCrossingStart; };
    inline const int& getReferenceOfClosestZeroCrossingEnd() { return mClosestZeroCrossingEnd; };
    inline const int& getReferenceForCycleLenHint() { return mCycleLenHint; };
    inline int getSizeOfOrigAudioData(){ return static_cast<int>(mOrigAudioData.size()); };
    inline int getSizeOfResampledCycles(){ return static_cast<int>(mResampledCycles.size()); };
    inline int getCycleLenHint(){ return mCycleLenHint; };

    inline void readAudioFileAndCopyToVector(juce::String filePath){
        juce::File audioFile = juce::File(filePath);

        mOriginalFileName = audioFile.getFileName();
        
        pFileHandler->readAudioFileAndCopyToVector(audioFile, mOrigAudioData);
    }

    void calculateZeroCrossingsAndUpdateVectors();

private:
    int mStartSampleIndex = 0;
    int mCycleLenHint = DEFAULT_CYCLE_LEN_HINT;
    int mClosestZeroCrossingStart = 0;
    int mClosestZeroCrossingEnd = 0;
    int mSelectedBand = 0;

    std::vector<bool> mZeroCrossings;
    std::vector<bool> mVectorThatShowsWhichSamplesAreCommitted;

    std::vector<float> mOrigAudioData;
    std::vector<float> mResampledCycles;
    std::vector<float> mPolarCycles;
    std::array<std::vector<float>, N_WT_BANDS> mResynthesizedCycles;
    
    juce::String mOriginalFileName;
    
    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
};


