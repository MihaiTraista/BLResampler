/*
  ==============================================================================

    DataModel.cpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#include "DataModel.hpp"

DataModel::DataModel(){}

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

    // create the three temp vectors
    std::vector<float> resampled = std::vector<float>(WTSIZE, 0.0f);

    pResampler->resizeCycle(origCycle, resampled);

    mResampledCycles.insert(mResampledCycles.end(), resampled.begin(), resampled.end());

    addResynthesizedCycle(resampled);
}

void DataModel::addResynthesizedCycle(const std::vector<float>& resampledCycle){
    std::vector<float> polarValues = std::vector<float>(WTSIZE * 2, 0.0f);
    std::vector<float> resynthesized = std::vector<float>(WTSIZE, 0.0f);

    Fourier::fillDftPolar(resampledCycle, polarValues);

    // harmonicsLimit = 512 because Niquist(22050) / freq(43) = 512 (1024 samples is 43 Hz)

    for(int band = 0; band < N_WT_BANDS; band++){
        float harmonicsLimit = 22050.0f / baseFrequencies[band];

        Fourier::idft(polarValues, resynthesized, harmonicsLimit, 50.0f, 1);

        mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(), resynthesized.begin(), resynthesized.end());
    }

    mPolarCycles.insert(mPolarCycles.end(), polarValues.begin(), polarValues.end());
}
