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
       int sizeOfOrigAudioData,
       int cycleLenHint);

    ~UI() override;

    void paint(juce::Graphics&) override;
        
    void resized() override;
    
    inline void buttonClicked(juce::Button* button) override {};

    inline void setRangeOfStartSampleIndexSlider(int newRange, bool shouldGetValueToHalfTheRange){
        mStartSampleIndexSlider.setRange(0, newRange);
        if(shouldGetValueToHalfTheRange)
            mStartSampleIndexSlider.setValue(newRange / 2, juce::sendNotification);
    };
    
    inline void setTextForResampledLengthLabel(juce::String newText){
        mResampledLengthLabel.setText(newText, juce::dontSendNotification);
    };
    
    inline void setRangeOfResampledZoomSlider(int newMaxValue) {
        mResampledZoomSlider.setRange(0, newMaxValue, 1);
        mResampledZoomSlider.setMaxValue(newMaxValue, juce::dontSendNotification);
    };

    inline void setEventConfirmationLabelTextAndVisibility(const juce::String& newText, bool isVisible) {
        mEventConfirmationLabel.setText(newText, juce::dontSendNotification);
        mEventConfirmationLabel.setVisible(isVisible);
    };
    
    inline int getStartSampleIndexSliderValue(){ return mStartSampleIndexSlider.getValue(); };
    inline int getCycleLenHintSliderValue(){ return mCycleLenHintSlider.getValue(); };
    inline int getBandSliderValue(){ return mBandSlider.getValue(); };
    inline bool getModeResynthesizedButtonState(){ return mModeResynthesizedButton.getToggleState(); };
    inline bool getModeOrigButtonState(){ return mModeOrigButton.getToggleState(); };
    inline bool getModeResampledButtonState(){ return mModeResampledButton.getToggleState(); };
    inline int getResampledZoomSliderMin(){ return mResampledZoomSlider.getMinValue(); };
    inline int getResampledZoomSliderMax(){ return mResampledZoomSlider.getMaxValue(); };

private:
    inline void sliderValueChanged(juce::Slider* slider) override {
        mEventHandler->handleSliderValueChanged(slider);
    };
    
    inline void comboBoxChanged(juce::ComboBox* box) override {};
    
    void addSlidersButtonsAndLabels(int sizeOfOrigAudioData, int cycleLenHint);
    
    void addIds();
    
    EventInterface* mEventHandler = nullptr;

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
