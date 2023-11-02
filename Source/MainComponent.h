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
#include "./DataModel/DataModel.hpp"

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
    void newFileWasDropped(juce::File audioFile, bool isResampled);
    
    // a reference to this function will be passed to the worker thread which will inform us when it finished a job
    void workerThreadFinishedJobCallback(){
        // you could update the UI here, but
        // you need to use a MessageManagerLock object to make sure it's thread-safe
        // for now we update the UI directly in handleCommitButton()
        
//        mUI.setRangeOfResampledZoomSlider(mDataModel.getSizeOfResynthesizedCycles() / WTSIZE);
//        updateLengthInfoLabel();
//        mUI.setEventConfirmationLabelTextAndVisibility("Cycle Committed!", true);
//        repaint();
    };
    
    void handleSliderValueChanged(juce::Slider* slider) override;
    void handleButtonClicked(juce::Button* button) override;
    void handleComboBoxChanged(juce::ComboBox* box) override;

private:
    void addSlidersButtonsAndLabels();
    void handleCommitButton();
    void updateVectors();
    
    DataModel mDataModel;

    UI mUI;
    
    PlaybackStates mPlaybackState = PlaybackStates::Stopped;

    std::unique_ptr<FileHandler> pFileHandler = std::make_unique<FileHandler>();
    std::unique_ptr<Resampler> pResampler = std::make_unique<Resampler>();
    std::unique_ptr<ZeroCrossingFinder> pZeroCrossingFinder = std::make_unique<ZeroCrossingFinder>();
    std::unique_ptr<Playback> pPlayback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
