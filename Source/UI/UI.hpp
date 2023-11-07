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
#include "../DragDropArea/DragDropArea.hpp"

class UI :  public juce::Component,
            private juce::Slider::Listener,
            public juce::Button::Listener,
            private juce::ComboBox::Listener
{
public:
    enum Modes{
        ORIG,
        RESAMPLED,
        RESYNTHESIZED
    };

    UI(EventInterface* handler,
       const std::vector<float>& audioVector,
       const std::vector<bool>& zeroCrossings,
       const std::vector<bool>& vectorThatShowsWhichSamplesAreCommitted,
       const int& closestZeroCrossingStartPointer,
       const int& closestZeroCrossingEndPointer,
       int sizeOfOrigAudioData,
       int cycleLenHint,
       std::function<void(juce::File, bool)> newFileWasDroppedReference);

    ~UI() override;

    void paint(juce::Graphics&) override;
        
    void resized() override;
    
    // EVENT LISTENER
    inline void buttonClicked(juce::Button* button) override {
        mEventHandler->handleButtonClicked(button);
    };

    // SETTERS
    inline void setRangeOfStartSampleIndexSlider(int newRange, bool shouldGetValueToHalfTheRange){
        mStartSampleIndexSlider.setRange(0, newRange >= 1 ? newRange : 1);
        if(shouldGetValueToHalfTheRange)
            mStartSampleIndexSlider.setValue(newRange / 2, juce::sendNotification);
    };
    
    inline void updateLengthInfoLabel(int len){
        juce::String labelText = "Resampled Buffer Length: " + juce::String(len) + " samples, " + juce::String(len / static_cast<float>(WTSIZE)) + " cycles";
        mResampledLengthLabel.setText(labelText, juce::dontSendNotification);
    };
    
    inline void setRangeOfResampledZoomSlider(int newMaxValue) {
        mResampledZoomSlider.setRange(0, newMaxValue, 1);
        mResampledZoomSlider.setMaxValue(newMaxValue, juce::dontSendNotification);
    };
    
    inline void setResampledZoomSliderMaxValue(int val){
        mResampledZoomSlider.setMaxValue(val);
    };

    inline void setEventConfirmationLabelTextAndVisibility(const juce::String& newText, bool isVisible) {
        mEventConfirmationLabel.setText(newText, juce::dontSendNotification);
        mEventConfirmationLabel.setVisible(isVisible);
    };
    
    inline void setLargeWaveformVector(const std::vector<float>& mAV,
                                       int displayStartSample,
                                       int displayLengthInSamples,
                                       bool showZeroCrossings){
        mLargeWaveform.setAudioVector(mAV, displayStartSample, displayLengthInSamples, showZeroCrossings);
    };
    
    inline void setSmallWaveformVector(const std::vector<float>& mAV,
                                       int displayStartSample,
                                       int displayLengthInSamples,
                                       bool showZeroCrossings){
        mSmallWaveform.setAudioVector(mAV, displayStartSample, displayLengthInSamples, showZeroCrossings);
    };
    
    inline void setSmallWaveformStart(int val){ mSmallWaveform.setDisplayStartSample(val); };
    inline void setLargeWaveformStart(int val){ mLargeWaveform.setDisplayStartSample(val); };
    inline void setSmallWaveformLength(int val){ mSmallWaveform.setDisplayLengthInSamples(val); };
    inline void setLargeWaveformLength(int val){ mLargeWaveform.setDisplayLengthInSamples(val); };
    inline void setStartSampleIndexSlider(int val){
        mStartSampleIndexSlider.setValue(val, juce::sendNotification);
    };

    void setMode(Modes mode);

    // NAVIGATION METHODS
    inline void goToPrevCycle(int actualCycleLen){
        int newVal = mStartSampleIndexSlider.getValue() - actualCycleLen;
        if(newVal < 0)
            newVal = 0;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    };

    inline void goToNextCycle(int actualCycleLen, int origVectorSize){
        int newVal = mStartSampleIndexSlider.getValue() + actualCycleLen;
        if(newVal > origVectorSize - actualCycleLen)
            newVal = origVectorSize - actualCycleLen;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    };
    
    inline void goToPrevSample(){
        int newVal = mStartSampleIndexSlider.getValue() - 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    }
    
    inline void goToNextSample(){
        int newVal = mStartSampleIndexSlider.getValue() + 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    }
    
    // BUTTON TRIGGERS
    inline void triggerClickPrevCycleButton(){ mPrevCycleButton.triggerClick(); };
    inline void triggerClickNextCycleButton(){ mNextCycleButton.triggerClick(); };
    inline void triggerClickPrevSampleButton(){ mPrevSampleButton.triggerClick(); };
    inline void triggerClickNextSampleButton(){ mNextSampleButton.triggerClick(); };
    inline void triggerClickModeResampled(){ mModeResampledButton.triggerClick(); };
    inline void triggerClickModeOrig(){ mModeOrigButton.triggerClick(); };
    inline void triggerClickCommitButton(){ mCommitButton.triggerClick(); };

    // ZOOM
    inline void zoomOutOneUnit(){
        int newVal = mCycleLenHintSlider.getValue() - 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
    };

    inline void zoomInOneUnit(){
        int newVal = mCycleLenHintSlider.getValue() + 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
    };
    
    // GETTERS
    inline int getStartSampleIndexSliderValue(){ return mStartSampleIndexSlider.getValue(); };
    inline int getCycleLenHintSliderValue(){ return mCycleLenHintSlider.getValue(); };
    inline int getBandSliderValue(){ return mBandSlider.getValue(); };
    inline bool getModeResynthesizedButtonState(){ return mModeResynthesizedButton.getToggleState(); };
    inline bool getModeOrigButtonState(){ return mModeOrigButton.getToggleState(); };
    inline bool getModeResampledButtonState(){ return mModeResampledButton.getToggleState(); };
    inline int getResampledZoomSliderMin(){ return mResampledZoomSlider.getMinValue(); };
    inline int getResampledZoomSliderMax(){ return mResampledZoomSlider.getMaxValue(); };
    inline bool getPlayButtonToggleState(){ return mPlayButton.getToggleState(); };
    inline int getLargeWaveformStartSampleIndex(){ return mLargeWaveform.getDisplayStartSample(); };
    inline Modes getMode(){ return mMode; };

private:
    // EVENT LISTENERS
    inline void sliderValueChanged(juce::Slider* slider) override {
        mEventHandler->handleSliderValueChanged(slider);
    };
    
    inline void comboBoxChanged(juce::ComboBox* box) override {
        mEventHandler->handleComboBoxChanged(box);
    };
        
    // OTHER PRIVATE METHODS
    void addSlidersButtonsAndLabels(int sizeOfOrigAudioData, int cycleLenHint);
    void addIds();
    
    // MEMBERS
    EventInterface* mEventHandler = nullptr;
    
    WaveformDisplay mLargeWaveform;
    WaveformDisplay mSmallWaveform;
    
    DragDropArea mDragDropAreaOriginal;
    DragDropArea mDragDropAreaResampled;

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
    
    Modes mMode = Modes::ORIG;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UI)
};
