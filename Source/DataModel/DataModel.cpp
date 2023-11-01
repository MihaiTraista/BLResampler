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

