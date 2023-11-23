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

struct Task {
    std::vector<float> resampledCycle;
};

class DataModel
{
public:
    DataModel(std::function<void()>);
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
    
    inline void storeDefaultCelloBinaryFileInOrigAudioData(){
        pFileHandler->storeDefaultCelloBinaryFileInOrigAudioData(mOrigAudioData);
    }

    inline void readFileAndStoreDataInResampledCycles(juce::File audioFile){
        pFileHandler->readAudioFileAndCopyToVector(audioFile, mResampledCycles);
    };

    inline void deleteLastCycle(){
        // delete the last 1024 samples from the vectors
        if (mResampledCycles.size() >= WTSIZE) {
            mResampledCycles.erase(mResampledCycles.end() - WTSIZE, mResampledCycles.end());
        }
        for(auto& wt : mResynthesizedCycles){
            if (wt.size() >= WTSIZE) {
                wt.erase(wt.end() - WTSIZE, wt.end());
            }
        }
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
    
    inline void exportFiles(){
        pFileHandler->exportFiles(mOrigAudioData,
                                  mResampledCycles,
                                  mPolarCycles,
                                  mAmpOfOriginalCycles,
                                  mResynthesizedCycles,
                                  mOriginalFileName);
    };
    
    inline void clearAllResampledAndResynthesized(){
        mResampledCycles.clear();
        mPolarCycles.clear();

        for(int i = 0; i < N_WT_BANDS; i++){
            mResynthesizedCycles[i].clear();
        }

        mVectorThatShowsWhichSamplesAreCommitted.assign(mVectorThatShowsWhichSamplesAreCommitted.size(), false);
        mVectorThatShowsWhichSamplesAreCommitted.clear();
    }
    
    void commit();
    void performDFTandAppendResynthesizedCycleForAllBands(std::vector<float>& resampledCycle);

private:
    void startWorkerThread();
    void workerThreadFunction();

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
    std::vector<float> mAmpOfOriginalCycles;
    std::array<std::vector<float>, N_WT_BANDS> mResynthesizedCycles;
    
    juce::String mOriginalFileName = "no_name";
    
    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
    
    std::vector<float> mWT = std::vector<float>(WTSIZE, 0.0f);
    std::vector<float> mBLWT = std::vector<float>(WTSIZE, 0.0f);
    
    std::atomic<bool> isWorkerThreadBusy = false;
    std::thread workerThread;
    std::vector<float> mTempResampledCycle = std::vector<float>(WTSIZE, 0.0f);
    std::vector<float> mTempPolar = std::vector<float>(WTSIZE * 2, 0.0f);
    std::array<std::vector<float>, N_WT_BANDS> mTempResynthesized;
    
    std::queue<Task> taskQueue;
    std::mutex queueMutex;
    
    std::function<void()> workerThreadFinishedJobCallback;
};


