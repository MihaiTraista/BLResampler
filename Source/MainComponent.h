#pragma once

#include <JuceHeader.h>
#include <vector>

#include "./WaveformDisplay/WaveformDisplay.hpp"
#include "./FileHandler/FileHandler.hpp"
#include "./Resampler/Resampler.hpp"
#include "./ZeroCrossingFinder/ZeroCrossingFinder.hpp"
#include "./Globals/Globals.hpp"
#include "./Fourier/Fourier.hpp"
#include "./Playback/Playback.hpp"

class MainComponent  :  public juce::AudioAppComponent,
                        public juce::FileDragAndDropTarget,
                        private juce::Slider::Listener,
                        public juce::Button::Listener,
                        public juce::KeyListener,
                        private juce::ComboBox::Listener
{
public:
    enum class PlaybackStates{
        Stopped,
        PlayingResampled,
        PlayingResynthesized
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

    inline bool isInterestedInFileDrag (const juce::StringArray& files) override { return true; };
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    inline void fileDragEnter (const juce::StringArray& files, int x, int y) override {};
    inline void fileDragExit (const juce::StringArray& files) override {};
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* box) override;
    
    void addSlidersButtonsAndLabels();
    void updateLengthInfoLabel();
    void handleCommitButton();
    void updateBufferAndRecalculateZeroCrossings(juce::File& audioFile);
    void normalizeBuffer(juce::AudioBuffer<float>& buffer);

    int mStartSampleIndex = 0;
    int mCycleLenHint = 600;
    int mClosestZeroCrossingStart = 0;
    int mClosestZeroCrossingEnd = 0;

    std::vector<bool> mZeroCrossings;
    std::vector<bool> mVectorThatShowsWhichSamplesAreCommitted;

    std::vector<float> mResampledCycles;
    std::vector<float> mPolarCycles;
    std::vector<float> mResynthesizedCycles;

    juce::AudioBuffer<float> mAudioFileBuffer;
    
    PlaybackStates mPlaybackState = PlaybackStates::Stopped;

    juce::Slider mStartSampleIndexSlider;
    juce::Slider mCycleLenHintSlider;
    juce::Label mCycleLenHintSliderLabel;
    juce::TextButton mCommitButton;
    juce::TextButton mSaveResampledFileButton;
    juce::TextButton mClearResampledCyclesButton;
    juce::TextButton mNormalizeButton;
    juce::Label mResampledLengthLabel;
    juce::Label mEventConfirmationLabel;
    juce::Label mInstructionsLabel;
    juce::ComboBox mResampledCycleLengthComboBox;
    juce::Label mResampledCycleLengthComboBoxLabel;
    juce::TextButton mPlayResampledButton;
    juce::TextButton mPlayResynthesizedButton;

    WaveformDisplay mWaveformDisplay;
    WaveformDisplay mOriginalWaveform;
    
    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
    std::unique_ptr<Playback> pPlayback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
