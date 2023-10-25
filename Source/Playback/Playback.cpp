#include "Playback.hpp"

Playback::Playback(){}

void Playback::readSamplesFromVector(const juce::AudioSourceChannelInfo& bufferToFill, const std::vector<float>& vectorToReadFrom)
{
    if(vectorToReadFrom.size() < 1)
        return;
    
    int numChannels = bufferToFill.buffer->getNumChannels();
    int numSamples = bufferToFill.numSamples;
    
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    for (int sample = 0; sample < numSamples; ++sample){
        mSampleCounter = (++mSampleCounter) % vectorToReadFrom.size();
        leftChannel[sample] = vectorToReadFrom[mSampleCounter];
    }

    for (int sample = 0; sample < numSamples; ++sample){
        rightChannel[sample] = leftChannel[sample];
    }
}
