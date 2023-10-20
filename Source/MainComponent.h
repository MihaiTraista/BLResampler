#pragma once

#include <JuceHeader.h>
#include <vector>

#include "./WaveformDisplay/WaveformDisplay.hpp"
#include "./FileHandler/FileHandler.hpp"
#include "./Resampler/Resampler.hpp"
#include "./ZeroCrossingFinder/ZeroCrossingFinder.hpp"


class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    int mStartSampleIndex = 0;
    int mCycleLenHint = 200;
    int mClosestZeroCrossingStart = 0;
    int mClosestZeroCrossingEnd = 0;

    std::vector<bool> mZeroCrossings;
    std::vector<bool> mVectorThatShowsWhichSamplesAreCommitted;
    std::vector<float> mResampledCycles;

    juce::AudioBuffer<float> mAudioFileBuffer;

    WaveformDisplay mWaveformDisplay;
    WaveformDisplay mOriginalWaveform;
    
    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
