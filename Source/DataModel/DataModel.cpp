/*
  ==============================================================================

    DataModel.cpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#include "DataModel.hpp"

DataModel::DataModel(){
    for(auto& vec : mTempResynthesized)
        vec = std::vector<float>(WTSIZE, 0.0f);
}

DataModel::~DataModel(){}

void DataModel::calculateZeroCrossingsAndUpdateVectors(){
    mClosestZeroCrossingStart = 0;
    mClosestZeroCrossingEnd = 0;
    mSelectedBand = 0;
    mStartSampleIndex = 0;
    mCycleLenHint = DEFAULT_CYCLE_LEN_HINT;

    mZeroCrossings.clear();
    mVectorThatShowsWhichSamplesAreCommitted.resize(mOrigAudioData.size(), false);

    mResampledCycles.clear();
    mPolarCycles.clear();
    
    for(int i = 0; i < N_WT_BANDS; i++){
        mResynthesizedCycles[i].clear();
    }

    pZeroCrossingFinder->calculateZeroCrossings(mOrigAudioData, mZeroCrossings);

    pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                mClosestZeroCrossingStart,
                                                                mClosestZeroCrossingEnd,
                                                                mStartSampleIndex,
                                                                mCycleLenHint);
    
}

void DataModel::commit(){

    const float* mAudioBufferData = mOrigAudioData.data();

    int originalLengthOfCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;

    if(originalLengthOfCycle < 3)
        return;

    // set to true all indexes from the original file that are going to be used
    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
    }

    // make a temp origCycle and copy the sampled from orig file
    std::vector<float> origCycle(originalLengthOfCycle, 0.0f);
    for(int i = 0; i < originalLengthOfCycle; ++i){
        origCycle[i] = mAudioBufferData[i + mClosestZeroCrossingStart];
    }

    pResampler->resizeCycle(origCycle, mTempResampledCycle);

    mResampledCycles.insert(mResampledCycles.end(), mTempResampledCycle.begin(), mTempResampledCycle.end());

    try {
        performDFTandAppendResynthesizedCycleForAllBands();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        juce::JUCEApplication::quit();
    }
}

void DataModel::performDFTandAppendResynthesizedCycleForAllBands(){
    if (!isWorkerThreadBusy.load()) {
        isWorkerThreadBusy.store(true);
        if (workerThread.joinable()) {
            workerThread.join();
        }

        workerThread = std::thread([this]() {
            Fourier::fill(mTempResampledCycle, mTempPolar, mTempResynthesized);
            
            for(int band = 0; band < N_WT_BANDS; band++){
                mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(),
                                                  mTempResynthesized[band].begin(),
                                                  mTempResynthesized[band].end());
            }

            mPolarCycles.insert(mPolarCycles.end(), mTempPolar.begin(), mTempPolar.end());
            
            isWorkerThreadBusy.store(false);
        });
        
//        workerThread = std::thread(&Fourier::fill,
//                                   std::cref(mTempResampledCycle),
//                                   std::ref(mTempPolar),
//                                   std::ref(mTempResynthesized),
//                                   std::ref(isWorkerThreadBusy));
//
//        for(int band = 0; band < N_WT_BANDS; band++){
//            mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(),
//                                              mTempResynthesized[band].begin(),
//                                              mTempResynthesized[band].end());
//        }
//
//        mPolarCycles.insert(mPolarCycles.end(), mTempPolar.begin(), mTempPolar.end());
    } else {
        throw std::runtime_error("!! ==> APP QUITTED ==>> Could not commit this cycles because the worker thread hasn't yet finished its job");
    }
}
