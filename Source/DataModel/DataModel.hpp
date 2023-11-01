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
#include "../Fourier/Fourier.hpp"

class DataModel
{
public:
    DataModel();
    ~DataModel();
        
    //  SETTERS
    inline void setStartSampleIndex(int value) { mStartSampleIndex = value; };
    inline void setCycleLenHint(int newHint){ mCycleLenHint = newHint; };
    inline void setSelectedBand(int val){ mSelectedBand = val; };
    inline void setOrigFileName(juce::String newName){ mOriginalFileName = newName; };

    //  GETTERS
    inline const std::vector<float>& getReferenceForOrigAudioDataVector() { return mOrigAudioData; };
    inline const std::vector<float>& getReferenceForResampledCyclesVector() { return mResampledCycles; };
    inline const std::vector<float>& getReferenceForResynthesizedCyclesVector() {
        return mResynthesizedCycles[mSelectedBand]; };
    inline const std::vector<bool>& getReferenceForZeroCrossingsVector() { return mZeroCrossings; };
    inline const std::vector<bool>& getReferenceForVectorThatShowsWhichSamplesAreCommitted() {
        return mVectorThatShowsWhichSamplesAreCommitted; };
    inline const int& getReferenceOfClosestZeroCrossingStart() { return mClosestZeroCrossingStart; };
    inline const int& getReferenceOfClosestZeroCrossingEnd() { return mClosestZeroCrossingEnd; };
    inline const int& getReferenceForCycleLenHint() { return mCycleLenHint; };

    inline int getStartSampleIndex() const { return mStartSampleIndex; };
    inline int getSizeOfOrigAudioData(){ return static_cast<int>(mOrigAudioData.size()); };
    inline int getSizeOfResampledCycles(){ return static_cast<int>(mResampledCycles.size()); };
    inline int getSizeOfResynthesizedCycles(){ return static_cast<int>(mResynthesizedCycles[0].size()); };
    inline int getActualCycleLen(){ return mClosestZeroCrossingEnd - mClosestZeroCrossingStart; };
    inline int getCycleLenHint(){ return mCycleLenHint; };

    //  OTHER METHODS
    inline void readFileAndStoreDataInOrigAudioData(juce::File audioFile){
        mOriginalFileName = audioFile.getFileName();
        
        pFileHandler->readAudioFileAndCopyToVector(audioFile, mOrigAudioData);
    };

    inline void readFileAndStoreDataInResampledCycles(juce::File audioFile){
        pFileHandler->readAudioFileAndCopyToVector(audioFile, mResampledCycles);
    };
    
    inline int clearVectorsAndResynthesizeAllCycles(){
        // clear
        mOrigAudioData.clear();
        mPolarCycles.clear();
        for(int i = 0; i < N_WT_BANDS; i++){
            mResynthesizedCycles[i].clear();
        }

        std::vector<float> resampledCycle = std::vector<float>(WTSIZE, 0.0f);

        size_t nCycles = mResampledCycles.size() / WTSIZE;

        for(int cycleIndex = 0; cycleIndex < nCycles; cycleIndex++){
            for(int i = 0; i < WTSIZE; i++){
                resampledCycle[i] = mResampledCycles[cycleIndex * WTSIZE + i];
            }

            performDFTandAppendResynthesizedCycleForAllBands(resampledCycle);
        }

        return static_cast<int>(nCycles);
    };

    void calculateZeroCrossingsAndUpdateVectors();
    
    inline void findClosestZeroCrossings(){
        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                    mClosestZeroCrossingStart,
                                                                    mClosestZeroCrossingEnd,
                                                                    mStartSampleIndex,
                                                                    mCycleLenHint);
    };
    
    void commit();
    void performDFTandAppendResynthesizedCycleForAllBands(const std::vector<float>& resampledCycle);

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


