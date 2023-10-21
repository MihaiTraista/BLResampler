#include "Playback.hpp"

Playback::Playback(std::vector<float>& resampledCycles,
                   std::vector<float>& polarCycles,
                   std::vector<float>& resynthesizedCycles):
    mResampledCycles(resampledCycles),
    mPolarCycles(polarCycles),
    mResynthesizedCycles(resynthesizedCycles)
{}

void Playback::audioCallback(const juce::AudioSourceChannelInfo& bufferToFill, std::vector<float>& vectorToReadFrom)
{
    if(mResampledCycles.size() < 1)
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
