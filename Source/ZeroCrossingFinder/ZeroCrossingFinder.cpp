/*
  ==============================================================================

    ZeroCrossingFinder.cpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#include "ZeroCrossingFinder.hpp"

ZeroCrossingFinder::ZeroCrossingFinder(){}

ZeroCrossingFinder::~ZeroCrossingFinder(){}

void ZeroCrossingFinder::calculateZeroCrossings(const juce::AudioBuffer<float>& mAudioFileBuffer, std::vector<bool>& mZeroCrossings){
    int numSamples = mAudioFileBuffer.getNumSamples();
    int numOfZeroCrossings = 0;

    mZeroCrossings.resize(numSamples, false);

    int lookingRange = 10;
    for(int i = lookingRange; i < numSamples - lookingRange; ++i){
        bool found_zero_crossing = false;
        int zero_crossing_index = -1;

        // Assuming the buffer is mono for simplicity
        const float* data = mAudioFileBuffer.getReadPointer(0);

        // Check if the signal crosses zero by checking if the sign changes
        // between any two consecutive samples in the looking range
        for(int j = i - lookingRange; j < i + lookingRange - 1; ++j){
            if((data[j] < 0.0f && data[j + 1] > 0.0f) || (data[j] > 0.0f && data[j + 1] < 0.0f)){
                found_zero_crossing = true;
                zero_crossing_index = j + 1; // The index after the sign change
                break;
            }
        }

        if(found_zero_crossing){
            mZeroCrossings[zero_crossing_index] = true;
            numOfZeroCrossings ++;
        }
    }

    std::cout << "Found " << numOfZeroCrossings << " zero crossings, out of " << mAudioFileBuffer.getNumSamples() << " total samples" << std::endl;
}

void ZeroCrossingFinder::findClosestZeroCrossingsToCycleLenHint(const std::vector<bool>& mZeroCrossings,
                                                                int& mClosestZeroCrossingStart,
                                                                int& mClosestZeroCrossingEnd,
                                                                const int& mStartSampleIndex,
                                                                const int& mCycleLenHint){
    int startOfDisplay = mStartSampleIndex;
    int endofDisplay = mCycleLenHint * 2 + mStartSampleIndex;
    int rangeOfDisplay = endofDisplay - startOfDisplay;
    int cycleLenHintLeftIndex = startOfDisplay + rangeOfDisplay / 4;
    int cycleLenHintRightIndex = startOfDisplay + (rangeOfDisplay / 4) * 3;
    
    mClosestZeroCrossingStart = 0;
    mClosestZeroCrossingEnd = 0;

    int closestDistanceStart = rangeOfDisplay;
    int closestDistanceEnd = rangeOfDisplay;

    for(int i = startOfDisplay; i < endofDisplay; ++i){
        if(mZeroCrossings[i] == true){
            // Check if this zero crossing is closer to selectionLeftIndex than the current closest
            int distanceToStart = std::abs(i - cycleLenHintLeftIndex);
            if(distanceToStart < closestDistanceStart){
                mClosestZeroCrossingStart = i;
                closestDistanceStart = distanceToStart;
            }

            // Check if this zero crossing is closer to selectionRightIndex than the current closest
            int distanceToEnd = std::abs(i - cycleLenHintRightIndex);
            if(distanceToEnd < closestDistanceEnd){
                mClosestZeroCrossingEnd = i;
                closestDistanceEnd = distanceToEnd;
            }
        }
    }

    // Handle the case where no zero crossings were found
    if(mClosestZeroCrossingStart == -1 || mClosestZeroCrossingEnd == -1){
        // Handle error: no zero crossings found in range
        std::cerr << "No zero crossings found in range" << std::endl;
    }
}
