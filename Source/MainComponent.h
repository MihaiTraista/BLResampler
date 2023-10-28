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

#define DEFAULT_CYCLE_LEN_HINT (600)

class MainComponent  :  public juce::AudioAppComponent,
                        private juce::Slider::Listener,
                        public juce::Button::Listener,
                        public juce::KeyListener,
                        private juce::ComboBox::Listener
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
    void buttonClicked(juce::Button* button) override;

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    // a reference to this function will be sent to the DragDropArea child components
    void newFileWasDropped(bool isResampled);

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    
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
    
    PlaybackStates mPlaybackState = PlaybackStates::Stopped;

    juce::Slider mStartSampleIndexSlider;
    juce::Slider mCycleLenHintSlider;
    juce::Label mCycleLenHintSliderLabel;
    juce::Slider mBandSlider;
    juce::Label mBandSliderLabel;
    juce::Slider mResampledZoomSlider;
    juce::Label mResampledZoomSliderLabel;

    juce::TextButton mCommitButton;
    juce::TextButton mSaveButton;
    juce::TextButton mClearButton;
    juce::TextButton mDeleteButton;

    juce::Label mResampledLengthLabel;
    juce::Label mEventConfirmationLabel;
    juce::Label mInstructionsLabel;
    juce::ComboBox mCycleLengthComboBox;
    juce::Label mCycleLengthComboBoxLabel;

    juce::TextButton mPlayButton;

    juce::TextButton mModeOrigButton;
    juce::TextButton mModeResampledButton;
    juce::TextButton mModeResynthesizedButton;

    juce::TextButton mPrevCycleButton;
    juce::TextButton mPrevSampleButton;
    juce::TextButton mNextSampleButton;
    juce::TextButton mNextCycleButton;

    WaveformDisplay mLargeWaveform;
    WaveformDisplay mSmallWaveform;
    
    DragDropArea mDragDropAreaOriginal;
    DragDropArea mDragDropAreaResampled;

    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
    std::unique_ptr<Playback> pPlayback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
