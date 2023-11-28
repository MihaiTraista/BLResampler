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
{}

DataModel::~DataModel(){
    // Stop the worker thread safely
    if (workerThread.joinable()) {
        // Signal the worker thread to stop, for example by setting a flag
        isWorkerThreadBusy = false;

        // Join the thread
        workerThread.join();
    }
}

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
    
    int lenOfOriginalCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;

    if(lenOfOriginalCycle < 3)
        return;

    // set to true all indexes from the original file that are going to be used
    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
    }

    // make a temp origCycle and copy the sampled from orig file
    std::vector<float> origCycle(lenOfOriginalCycle, 0.0f);

    int maxAmp = copyAndNormalizeCycleFromAudioBufferDataAndNormalize(origCycle, lenOfOriginalCycle);

    mAmpOfOriginalCycles.insert(mAmpOfOriginalCycles.end(), WTSIZE, maxAmp);

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

int DataModel::copyAndNormalizeCycleFromAudioBufferDataAndNormalize(std::vector<float>& origCycle,
                                                                    const int& lenOfOriginalCycle){

    const float* mAudioBufferData = mOrigAudioData.data();
    
    float maxAmp = 0.0f;

    // normalize the cycle
    for(int i = 0; i < lenOfOriginalCycle; ++i){
        float val = mAudioBufferData[i + mClosestZeroCrossingStart];
        origCycle[i] = val;
        maxAmp = std::max(fabs(val), maxAmp);
    }
    
    float gainNormalizer = (1.0f / maxAmp) * 0.9f;
    for (float& sample : origCycle)
        sample *= gainNormalizer;
    
    return maxAmp;
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

        Fourier::execute(task.resampledCycle,
                         mPolarCycles,
                         mResynthesizedCycles);
    
        workerThreadFinishedJobCallback();
    }
}
