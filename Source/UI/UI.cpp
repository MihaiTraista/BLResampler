/*
  ==============================================================================

    UI.cpp
    Created: 31 Oct 2023
    Author:  Mihai Traista

  ==============================================================================
*/

#include "UI.hpp"

UI::UI(EventInterface* handler,
       const std::vector<float>& audioVector,
       const std::vector<bool>& zeroCrossings,
       const std::vector<bool>& vectorThatShowsWhichSamplesAreCommitted,
       const int& closestZeroCrossingStartPointer,
       const int& closestZeroCrossingEndPointer,
       int sizeOfOrigAudioData,
       int cycleLenHint)
:
    mEventHandler(handler),
    mLargeWaveform(audioVector,
                   zeroCrossings,
                   vectorThatShowsWhichSamplesAreCommitted,
                   closestZeroCrossingStartPointer,
                   closestZeroCrossingEndPointer,
                   0,
                   500),
    mSmallWaveform(audioVector, 0, 0)
{
    addAndMakeVisible(mLargeWaveform);
    addAndMakeVisible(mSmallWaveform);
    addSlidersButtonsAndLabels(sizeOfOrigAudioData, cycleLenHint);
    addIds();
}

UI::~UI(){}

void UI::setMode(Modes mode){
    mMode = mode;
    
    mModeOrigButton.setToggleState(mode == Modes::ORIG, juce::dontSendNotification);
    mModeResampledButton.setToggleState(mode == Modes::RESAMPLED, juce::dontSendNotification);
    mModeResynthesizedButton.setToggleState(mode == Modes::RESYNTHESIZED, juce::dontSendNotification);
    
    if(mode == Modes::ORIG){
        mResampledZoomSlider.setVisible(false);
        mResampledZoomSliderLabel.setVisible(false);
        mCycleLenHintSlider.setVisible(true);
        mCycleLenHintSliderLabel.setVisible(true);
        mBandSlider.setVisible(false);
        mBandSliderLabel.setVisible(false);
        mStartSampleIndexSlider.setVisible(true);
        mPrevCycleButton.setEnabled(true);
        mPrevSampleButton.setEnabled(true);
        mNextSampleButton.setEnabled(true);
        mNextCycleButton.setEnabled(true);
        mPrevCycleButton.setVisible(true);
        mPrevSampleButton.setVisible(true);
        mNextSampleButton.setVisible(true);
        mNextCycleButton.setVisible(true);
    } else if (mode == Modes::RESAMPLED){
        mResampledZoomSlider.setVisible(true);
        mResampledZoomSliderLabel.setVisible(true);
        mCycleLenHintSlider.setVisible(false);
        mCycleLenHintSliderLabel.setVisible(false);
        mBandSlider.setVisible(false);
        mBandSliderLabel.setVisible(false);
        mStartSampleIndexSlider.setVisible(false);
        mPrevCycleButton.setEnabled(false);
        mPrevSampleButton.setEnabled(false);
        mNextSampleButton.setEnabled(false);
        mNextCycleButton.setEnabled(false);
        mPrevCycleButton.setVisible(false);
        mPrevSampleButton.setVisible(false);
        mNextSampleButton.setVisible(false);
        mNextCycleButton.setVisible(false);
    } else if (mode == Modes::RESYNTHESIZED){
        mResampledZoomSlider.setVisible(true);
        mResampledZoomSliderLabel.setVisible(true);
        mCycleLenHintSlider.setVisible(false);
        mCycleLenHintSliderLabel.setVisible(false);
        mBandSlider.setVisible(true);
        mBandSliderLabel.setVisible(true);
        mStartSampleIndexSlider.setVisible(false);
        mPrevCycleButton.setEnabled(false);
        mPrevSampleButton.setEnabled(false);
        mNextSampleButton.setEnabled(false);
        mNextCycleButton.setEnabled(false);
        mPrevCycleButton.setVisible(false);
        mPrevSampleButton.setVisible(false);
        mNextSampleButton.setVisible(false);
        mNextCycleButton.setVisible(false);
    }
}

void UI::paint(juce::Graphics& g)
{
}

void UI::resized()
{
    float aAreaY = 0;
    float bAreaY = 150;
    float cAreaY = 550;
    float aAreaHeight = 150;
    float bAreaHeight = 400;
    float cAreaHeight = 50;
    float width = getWidth();
    float gap = 5;
    float buttonsYOffset = 50;
    float bigButtonWidth = 86;
    float bigButtonHeight = 30;
    
    mLargeWaveform.setBounds(0, bAreaY, width, bAreaHeight);
    mSmallWaveform.setBounds(0, cAreaY, width, cAreaHeight);
    
    mModeOrigButton.setBounds(gap, buttonsYOffset, bigButtonWidth, bigButtonHeight);
    mModeResampledButton.setBounds(gap + bigButtonWidth + 2, buttonsYOffset, bigButtonWidth, bigButtonHeight);
    mModeResynthesizedButton.setBounds(gap + bigButtonWidth * 2 + 4, buttonsYOffset, bigButtonWidth, bigButtonHeight);
    
    mPrevCycleButton.setBounds(gap, buttonsYOffset + 32, 20, 20);
    mPrevSampleButton.setBounds(gap + 22, buttonsYOffset + 32, 20, 20);
    mNextSampleButton.setBounds(gap + 44, buttonsYOffset + 32, 20, 20);
    mNextCycleButton.setBounds(gap + 66, buttonsYOffset + 32, 20, 20);

    // SLIDERS
    mBandSliderLabel.setBounds(gap + 140, buttonsYOffset + 55, 100, 10);
    mBandSlider.setBounds(gap, buttonsYOffset + 60, 262, 20);
    mCycleLenHintSliderLabel.setBounds(gap + 140, buttonsYOffset + 75, 100, 10);
    mCycleLenHintSlider.setBounds(gap, buttonsYOffset + 80, 262, 20);
    mResampledZoomSliderLabel.setBounds(gap + 140, buttonsYOffset + 75, 100, 10);
    mResampledZoomSlider.setBounds(gap, buttonsYOffset + 80, 262, 20);

    // the mStartSampleIndexSlider should overlap mSmallWaveform
    mStartSampleIndexSlider.setBounds(0, cAreaY, width, cAreaHeight);

    // Commit, Save, Clear, Delete
    mCommitButton.setBounds(553, buttonsYOffset, 58, 30);
    mSaveButton.setBounds(613, buttonsYOffset, 58, 30);
    mClearButton.setBounds(673, buttonsYOffset, 58, 30);
    mDeleteButton.setBounds(733, buttonsYOffset, 58, 30);
    
    mCycleLengthComboBox.setBounds(733, buttonsYOffset + 32, 58, 20);
    
    mEventConfirmationLabel.setBounds(0, bAreaY, width, 16);
    mResampledLengthLabel.setBounds(280, cAreaY - 10, width - 300, 10);
//    mInstructionsLabel.setBounds(10, 40, getWidth(), getHeight());

    mPlayButton.setBounds(360, buttonsYOffset, 80, 50);
}

void UI::addSlidersButtonsAndLabels(int sizeOfOrigAudioData, int cycleLenHint){
    // Start Sample Index Slider and Label
    mStartSampleIndexSlider.setSliderStyle(juce::Slider::LinearBar);
    // the audio file is already loaded, so we can set the range to the length of the file
    if(sizeOfOrigAudioData > 0){
        mStartSampleIndexSlider.setRange(0, sizeOfOrigAudioData - cycleLenHint * 2);
        mStartSampleIndexSlider.setValue(sizeOfOrigAudioData / 2, juce::sendNotification);
    } else {
        mStartSampleIndexSlider.setRange(0, 1, 1);
        mStartSampleIndexSlider.setValue(0);
    }
    
    mStartSampleIndexSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mStartSampleIndexSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGBA(255, 255, 255, 50));
    mStartSampleIndexSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    mStartSampleIndexSlider.addListener(this);
    addAndMakeVisible(&mStartSampleIndexSlider);
    
    // Cycle Len Hint Slider and Label
    mCycleLenHintSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mCycleLenHintSlider.setRange(10, 4000, 1);
    mCycleLenHintSlider.setSkewFactor(0.25);
    mCycleLenHintSlider.setValue(DEFAULT_CYCLE_LEN_HINT);
    mCycleLenHintSlider.addListener(this);
    addAndMakeVisible(&mCycleLenHintSlider);

    mCycleLenHintSliderLabel.setText("Zoom", juce::dontSendNotification);
    mCycleLenHintSliderLabel.setInterceptsMouseClicks(false, false);
    mCycleLenHintSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(&mCycleLenHintSliderLabel);

    // Band Slider and Label
    mBandSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mBandSlider.setRange(0, 9, 1);
    mBandSlider.setValue(0);
    mBandSlider.addListener(this);
    addAndMakeVisible(&mBandSlider);

    mBandSliderLabel.setText("Band", juce::dontSendNotification);
    mBandSliderLabel.setInterceptsMouseClicks(false, false);
    mBandSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(&mBandSliderLabel);
    
    mBandSlider.setVisible(false);
    mBandSliderLabel.setVisible(false);

    // Zoom Cycles
    mResampledZoomSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    mResampledZoomSlider.setRange(0, 1, 1);
    mResampledZoomSlider.setMinValue(0);
    mResampledZoomSlider.setMaxValue(1);

    mResampledZoomSlider.addListener(this);
    addAndMakeVisible(&mResampledZoomSlider);

    mResampledZoomSliderLabel.setText("Zoom", juce::dontSendNotification);
    mResampledZoomSliderLabel.setInterceptsMouseClicks(false, false);
    mResampledZoomSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(&mResampledZoomSliderLabel);
    
    mResampledZoomSlider.setVisible(false);
    mResampledZoomSliderLabel.setVisible(false);

    // Commit, Save, Clear, Delete
    mCommitButton.setButtonText("Commit");
    mCommitButton.addListener(this);
    addAndMakeVisible(mCommitButton);

    mSaveButton.setButtonText("Save");
    mSaveButton.addListener(this);
    addAndMakeVisible(mSaveButton);
    
    mClearButton.setButtonText("Clear");
    mClearButton.addListener(this);
    addAndMakeVisible(mClearButton);

    mDeleteButton.setButtonText("Delete");
    mDeleteButton.addListener(this);
    addAndMakeVisible(mDeleteButton);
    
    // Show Resampled Length Label
    mResampledLengthLabel.setText("Resampled Buffer Length: 0 samples, 0 cycles", juce::dontSendNotification);
    mResampledLengthLabel.setFont(juce::Font(12.0f));
    mResampledLengthLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(mResampledLengthLabel);
    
    // mEventConfirmationLabel
    mEventConfirmationLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    // the text will be set before the label is made visible
    // it will be either "Cycle Committed!" or "File Saved!"
    mEventConfirmationLabel.setText("", juce::dontSendNotification);
    mEventConfirmationLabel.setFont(juce::Font("Arial", "Bold", 16.0f));
    mEventConfirmationLabel.setJustificationType(juce::Justification::centredTop);
    addAndMakeVisible(mEventConfirmationLabel);
    mEventConfirmationLabel.setVisible(false);
    
    // mInstructionsLabel
    mInstructionsLabel.setText("Drag & Drop an audio file\n"
                               "Keyboard Shortcuts\n"
                               "G - previous cycle:\n"
                               "H - previous sample\n"
                               "J - next sample\n"
                               "K - next cycle\n"
                               "U - zoom out\n"
                               "I - zoom in\n"
                               "C - commit",
                               juce::dontSendNotification);
    mInstructionsLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    mInstructionsLabel.setJustificationType(juce::Justification::topLeft);
    mInstructionsLabel.setEditable(false, false, true);
    mInstructionsLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(mInstructionsLabel);
    
    // Cycle Len ComboBox
    mCycleLengthComboBox.addItem("512", 1);
    mCycleLengthComboBox.addItem("1024", 2);
    mCycleLengthComboBox.addItem("2048", 3);
    mCycleLengthComboBox.addItem("4096", 4);
    mCycleLengthComboBox.setSelectedId(2);
    mCycleLengthComboBox.addListener(this);
    addAndMakeVisible(mCycleLengthComboBox);
    
    // Cycle Len Combo Box Label
    mCycleLengthComboBoxLabel.setText("Cycle Len", juce::dontSendNotification);
    mCycleLengthComboBoxLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    mCycleLengthComboBoxLabel.attachToComponent(&mCycleLengthComboBox, true);
    addAndMakeVisible(mCycleLengthComboBoxLabel);
    
    // Play Button
    mPlayButton.setButtonText("Play");
    mPlayButton.setToggleable(true);
    mPlayButton.setClickingTogglesState(true);
    mPlayButton.addListener(this);
    addAndMakeVisible(mPlayButton);
    
    // Mode Tabs
    mModeOrigButton.setButtonText("Original");
    mModeOrigButton.setClickingTogglesState(true);
    mModeOrigButton.setToggleState(true, juce::dontSendNotification);
    mModeOrigButton.addListener(this);
    addAndMakeVisible(mModeOrigButton);

    mModeResampledButton.setButtonText("Resampled");
    mModeResampledButton.setClickingTogglesState(true);
    mModeResampledButton.setToggleState(false, juce::dontSendNotification);
    mModeResampledButton.addListener(this);
    addAndMakeVisible(mModeResampledButton);

    mModeResynthesizedButton.setButtonText("Resynth");
    mModeResynthesizedButton.setClickingTogglesState(true);
    mModeResynthesizedButton.setToggleState(false, juce::dontSendNotification);
    mModeResynthesizedButton.addListener(this);
    addAndMakeVisible(mModeResynthesizedButton);
    
    // Prev Cycle, Prev Sample, Next Sample, Next Cycle Buttons
    mPrevCycleButton.setButtonText("<");
    mPrevCycleButton.setColour(juce::TextButton::textColourOffId, juce::Colours::yellow);
    mPrevCycleButton.addListener(this);
    addAndMakeVisible(mPrevCycleButton);

    mPrevSampleButton.setButtonText("<");
    mPrevSampleButton.addListener(this);
    addAndMakeVisible(mPrevSampleButton);

    mNextSampleButton.setButtonText(">");
    mNextSampleButton.addListener(this);
    addAndMakeVisible(mNextSampleButton);

    mNextCycleButton.setButtonText(">");
    mNextCycleButton.setColour(juce::TextButton::textColourOffId, juce::Colours::yellow);
    mNextCycleButton.addListener(this);
    addAndMakeVisible(mNextCycleButton);
}

void UI::addIds() {
    mStartSampleIndexSlider.setComponentID("mStartSampleIndexSlider");
    mCycleLenHintSlider.setComponentID("mCycleLenHintSlider");
    mCycleLenHintSliderLabel.setComponentID("mCycleLenHintSliderLabel");
    mBandSlider.setComponentID("mBandSlider");
    mBandSliderLabel.setComponentID("mBandSliderLabel");
    mResampledZoomSlider.setComponentID("mResampledZoomSlider");
    mResampledZoomSliderLabel.setComponentID("mResampledZoomSliderLabel");

    mCommitButton.setComponentID("mCommitButton");
    mSaveButton.setComponentID("mSaveButton");
    mClearButton.setComponentID("mClearButton");
    mDeleteButton.setComponentID("mDeleteButton");

    mResampledLengthLabel.setComponentID("mResampledLengthLabel");
    mEventConfirmationLabel.setComponentID("mEventConfirmationLabel");
    mInstructionsLabel.setComponentID("mInstructionsLabel");
    mCycleLengthComboBox.setComponentID("mCycleLengthComboBox");
    mCycleLengthComboBoxLabel.setComponentID("mCycleLengthComboBoxLabel");

    mPlayButton.setComponentID("mPlayButton");

    mModeOrigButton.setComponentID("mModeOrigButton");
    mModeResampledButton.setComponentID("mModeResampledButton");
    mModeResynthesizedButton.setComponentID("mModeResynthesizedButton");

    mPrevCycleButton.setComponentID("mPrevCycleButton");
    mPrevSampleButton.setComponentID("mPrevSampleButton");
    mNextSampleButton.setComponentID("mNextSampleButton");
    mNextCycleButton.setComponentID("mNextCycleButton");
}

