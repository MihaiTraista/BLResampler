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

void Playback::playLiveResynthesis(const juce::AudioSourceChannelInfo& bufferToFill, const std::vector<float>& polar){
    if(polar.size() < 1)
        return;
    
    int numChannels = bufferToFill.buffer->getNumChannels();
    int numSamples = bufferToFill.numSamples;
    
    float* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    float* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

//    Fourier::idft(polar, mLiveResynthesized);
    
    float amp, phase, roll;
    float harmonicLimit = 8.0f;
    float startRolloff = harmonicLimit - harmonicLimit * (50.0f / 100.0f);
    startRolloff = 200;

    for (int i = 0; i < numSamples; i++) {
        
        mLiveResynthesized[i] = 0;
        
        for (int h = 0; h < numSamples; h++) {
            amp = polar[h * 2];
            phase = polar[h * 2 + 1];
            
            // roll off harmonics from startRolloff to harmonicLimit and zero out all higher harmonics
            if (h < startRolloff){
                amp *= 1.0;
            } else if (h >= startRolloff && h < harmonicLimit){
                roll = 1 - ((float)h - startRolloff) / (harmonicLimit - startRolloff);
                roll *= roll; // square roll for exponential roll off
                amp *= roll;
            } else {
                amp *= 0.0;
            }


            float val = amp * cos(TWOPI * i * h / static_cast<float>(numSamples) + phase);

            mLiveResynthesized[i] += val;
        }
    }

    for (int sample = 0; sample < numSamples; ++sample){
        mSampleCounter = (++mSampleCounter) % numSamples;
        leftChannel[sample] = mLiveResynthesized[sample];
    }

    for (int sample = 0; sample < numSamples; ++sample){
        rightChannel[sample] = leftChannel[sample];
    }

}
