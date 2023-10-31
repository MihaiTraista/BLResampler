#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>

#include "./WaveformDisplay/WaveformDisplay.hpp"
#include "./FileHandler/FileHandler.hpp"
#include "./Resampler/Resampler.hpp"
#include "./ZeroCrossingFinder/ZeroCrossingFinder.hpp"
#include "./Globals/Globals.hpp"
#include "./Fourier/Fourier.hpp"
#include "./Playback/Playback.hpp"
#include "./DragDropArea/DragDropArea.hpp"
#include "./UI/UI.hpp"

class MainComponent  :  public juce::AudioAppComponent,
                        public juce::KeyListener,
                        public EventInterface
{
public:
    enum class PlaybackStates{
        Playing,
        Stopped
    };
    
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

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    // a reference to this function will be sent to the DragDropArea child components
    void newFileWasDropped(bool isResampled);
    
    void handleSliderValueChanged(juce::Slider* slider) override;
    void handleButtonClicked(juce::Button* button) override;
    void handleComboBoxChanged(juce::ComboBox* box) override;

private:
    void addSlidersButtonsAndLabels();
    void updateLengthInfoLabel();
    void handleCommitButton();
    void calculateZeroCrossingsAndUpdateVectors();
    void addResynthesizedCycle(const std::vector<float>& resampledCycle);
    
    int mStartSampleIndex = 0;
    int mCycleLenHint = DEFAULT_CYCLE_LEN_HINT;
    int mClosestZeroCrossingStart = 0;
    int mClosestZeroCrossingEnd = 0;
    int mSelectedBand = 0;

    std::vector<bool> mZeroCrossings;
    std::vector<bool> mVectorThatShowsWhichSamplesAreCommitted;

    std::vector<float> mOrigAudioData;
    std::vector<float> mResampledCycles;
    std::vector<float> mPolarCycles;
    std::array<std::vector<float>, N_WT_BANDS> mResynthesizedCycles;
    
    juce::String mOriginalFileName;
    
    WaveformDisplay mLargeWaveform;
    WaveformDisplay mSmallWaveform;
    
    DragDropArea mDragDropAreaOriginal;
    DragDropArea mDragDropAreaResampled;
    
    UI mUI;

    PlaybackStates mPlaybackState = PlaybackStates::Stopped;

    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
    std::unique_ptr<Playback> pPlayback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
