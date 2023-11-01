/*
  ==============================================================================

    WaveformDisplay.hpp
    Created: 11 Oct 2023 12:57:31pm
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay(const std::vector<float>& audioVector,
                    const std::vector<bool>& zeroCrossings,
                    const std::vector<bool>& vectorThatShowsWhichSamplesAreCommitted,
                    const int& closestZeroCrossingStartPointer,
                    const int& closestZeroCrossingEndPointer,
                    int displayStartSample,
                    int displayLengthInSamples);
    WaveformDisplay(const std::vector<float>& audioVector,
                    int displayStartSample,
                    int displayLengthInSamples);

    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    
    inline void setAudioVector(const std::vector<float>& mAV,
                               int displayStartSample,
                               int displayLengthInSamples,
                               bool showZeroCrossings){
        pAudioVector = &mAV;
        mDisplayStartSample = displayStartSample;
        mDisplayLengthInSamples = displayLengthInSamples;
        mShowZeroCrossings = showZeroCrossings;
    };

    inline void setDisplayStartSample(int val){ mDisplayStartSample = val; };
    inline void setDisplayLengthInSamples(int val){ mDisplayLengthInSamples = val; };
    inline int getDisplayStartSample(){ return mDisplayStartSample; };
    inline int getDisplayLengthInSamples(){ return mDisplayLengthInSamples; };

    void resized() override;

private:
    void drawWaveformAsEnvelopeFollower(juce::Graphics& g,
                                        int width,
                                        int height,
                                        int numSamplesToDisplay,
                                        const float* bufferToDraw);
    void drawWaveformSampleBySample(juce::Graphics& g,
                                    int width,
                                    int height,
                                    int numSamplesToDisplay,
                                    const float* bufferToDraw);
    void drawZeroCrossingCirclesAndHintLines(juce::Graphics& g,
                                             int width,
                                             int height,
                                             int numSamplesToDisplay,
                                             const float* bufferToDraw);
    void drawTeardropShape(juce::Graphics& g, int x, int y, int width, int height, bool centered, bool reversed);

    const std::vector<float>* pAudioVector = nullptr;
    const std::vector<bool>* pZeroCrossings = nullptr;
    const std::vector<bool>* pVectorThatShowsWhichSamplesAreCommitted = nullptr;
    const int* pClosestZeroCrossingStart = nullptr;
    const int* pClosestZeroCrossingEnd = nullptr;

    int mDisplayStartSample = 0;
    int mDisplayLengthInSamples = 500;

    bool mShowZeroCrossings = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
