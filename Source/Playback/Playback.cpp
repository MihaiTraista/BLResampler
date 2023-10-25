#include "Playback.hpp"

Playback::Playback(const std::vector<float>* pAV, int start, int length):
    pAudioVector(pAV),
    mReadingStart(start),
    mReadingLength(length)
{}

void Playback::readSamplesFromVector(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if(pAudioVector == nullptr || pAudioVector->size() < 2)
        return;
    
    int numSamples = bufferToFill.numSamples;
    
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    for (int sample = 0; sample < numSamples; ++sample){
        mSampleCounter = (++mSampleCounter) % mReadingLength;
        leftChannel[sample] = (*pAudioVector)[mSampleCounter + mReadingStart];
    }

    for (int sample = 0; sample < numSamples; ++sample){
        rightChannel[sample] = leftChannel[sample];
    }
}
