/*
  ==============================================================================

    UI.hpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../WaveformDisplay/WaveformDisplay.hpp"
#include "../EventInterface/EventInterface.hpp"
#include "../Globals/Globals.hpp"

class UI :  public juce::Component,
            private juce::Slider::Listener,
            public juce::Button::Listener,
            private juce::ComboBox::Listener
{
public:
    UI(EventInterface* handler,
       std::vector<float>& origAudioData,
       std::vector<float>& resampledCycles,
       std::vector<float>& polarCycles,
       int& cycleLenHint);
    ~UI() override;

    void paint(juce::Graphics&) override;
        
    void resized() override;
    
    inline void buttonClicked(juce::Button* button) override {};


private:
    inline void sliderValueChanged(juce::Slider* slider) override {};
    inline void comboBoxChanged(juce::ComboBox* box) override {};
    
    void addSlidersButtonsAndLabels();
    
    EventInterface* mEventHandler = nullptr;

    std::vector<float>& mOrigAudioData;
    std::vector<float>& mResampledCycles;
    std::vector<float>& mPolarCycles;
    
    int& mCycleLenHint;

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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UI)
};
