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
    WaveformDisplay(std::vector<float>* pAudioVector,
                    std::vector<bool>* zeroCrossings,
                    std::vector<bool>* vectorThatShowsWhichSamplesAreCommitted,
                    int displayStartSample,
                    int displayLengthInSamples,
                    int* closestZeroCrossingStartPointer,
                    int* closestZeroCrossingEndPointer);
    WaveformDisplay(std::vector<float>* pAudioVector,
                    int displayStartSample,
                    int displayLengthInSamples);

    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    
    inline void setAudioVector(std::vector<float>* pAV,
                               int displayStartSample,
                               int displayLengthInSamples,
                               bool showZeroCrossings){
        pAudioVector = pAV;
        mDisplayStartSample = displayStartSample;
        mDisplayLengthInSamples = displayLengthInSamples;
        mShowZeroCrossings = showZeroCrossings;
    };

    inline void setDisplayStartSample(int val){ mDisplayStartSample = val; };
    inline void setDisplayLengthInSamples(int val){ mDisplayLengthInSamples = val; };
    
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

    std::vector<float>* pAudioVector = nullptr;
    std::vector<bool>* pZeroCrossings = nullptr;
    std::vector<bool>* pVectorThatShowsWhichSamplesAreCommitted = nullptr;

    int mDisplayStartSample = 0;
    int mDisplayLengthInSamples = 500;
    int* pClosestZeroCrossingStart = nullptr;
    int* pClosestZeroCrossingEnd = nullptr;

    bool mShowZeroCrossings = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
