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
                    std::vector<bool>* zeroCrossings,
                    std::vector<bool>* vectorThatShowsWhichSamplesAreCommitted,
                    int* startSampleIndexPointer,
                    int* cycleLenHintPointer,
                    int* closestZeroCrossingStartPointer,
                    int* closestZeroCrossingEndPointer);
    WaveformDisplay(const std::vector<float>& audioVector);
    
    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
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


    const std::vector<float>& mAudioVector;
    const std::vector<bool>* pZeroCrossings = nullptr;
    const std::vector<bool>* pVectorThatShowsWhichSamplesAreCommitted = nullptr;

    int* pStartSampleIndex = 0;
    int* pCycleLenHint = 0;
    int* pClosestZeroCrossingStart;
    int* pClosestZeroCrossingEnd;

    bool mShowZeroCrossings = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
