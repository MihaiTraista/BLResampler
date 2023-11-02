/*
  ==============================================================================

    DataModel.cpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#include "DataModel.hpp"

DataModel::DataModel(std::function<void()> workerThreadFinishedJobCallbackRef):
    workerThreadFinishedJobCallback(workerThreadFinishedJobCallbackRef)
{
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

    Task task;
    
    task.resampledCycle = mTempResampledCycle;
    
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }

    startWorkerThread();

}

void DataModel::startWorkerThread() {
    if (!isWorkerThreadBusy.load()) {
        isWorkerThreadBusy.store(true);
        
        if (workerThread.joinable()) {
            workerThread.join();
        }

        workerThread = std::thread(&DataModel::workerThreadFunction, this);
    }
}

void DataModel::workerThreadFunction() {
    while (true) {
        Task task;
        
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (taskQueue.empty()) {
                isWorkerThreadBusy.store(false);
                return;
            }
            task = taskQueue.front();
            taskQueue.pop();
        }

        performDFTandAppendResynthesizedCycleForAllBands(task.resampledCycle);
        
        workerThreadFinishedJobCallback();
    }
}


void DataModel::performDFTandAppendResynthesizedCycleForAllBands(std::vector<float>& resampledCycle){
    
    Fourier::fill(resampledCycle, mTempPolar, mTempResynthesized);
    
    for(int band = 0; band < N_WT_BANDS; band++){
        mResynthesizedCycles[band].insert(mResynthesizedCycles[band].end(),
                                          mTempResynthesized[band].begin(),
                                          mTempResynthesized[band].end());
    }

    mPolarCycles.insert(mPolarCycles.end(), mTempPolar.begin(), mTempPolar.end());

}
