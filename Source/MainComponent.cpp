#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent():
    mWaveformDisplay(&mOrigAudioData,
                     &mZeroCrossings,
                     &mVectorThatShowsWhichSamplesAreCommitted,
                     &mStartSampleIndex,
                     &mCycleLenHint,
                     &mClosestZeroCrossingStart,
                     &mClosestZeroCrossingEnd),
    mOriginalWaveform(&mOrigAudioData),
    pPlayback(std::make_unique<Playback>())
{
    setSize (800, 600);

    // Enable drag-and-drop
    setInterceptsMouseClicks (true, true);
    setWantsKeyboardFocus (true);
    addKeyListener(this);
    
    addAndMakeVisible(mWaveformDisplay);
    addAndMakeVisible(mOriginalWaveform);

    // read audio file and display waveform
    // /Users/mihaitraista/5.Sound Libraries/fl1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav
    // /Users/mihaitraista/4.Projects/Coding/JUCE/CycleChopper/Resources/Cello_C2_1.wav
    juce::File audioFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/WebTenori/Resources/ForResampling/Flute-Long.wav");

    updateBufferAndRecalculateZeroCrossings(audioFile);
    
    addSlidersButtonsAndLabels();
    
    // ask for audio input permission
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::addSlidersButtonsAndLabels(){
    // Start Sample Index Slider and Label
    mStartSampleIndexSlider.setSliderStyle(juce::Slider::LinearBar);
    // the audio file is already loaded, so we can set the range to the length of the file
    if(mOrigAudioData.size() > 0){
        mStartSampleIndexSlider.setRange(0, mOrigAudioData.size() - mCycleLenHint * 2);
        mStartSampleIndexSlider.setValue(mOrigAudioData.size() / 2, juce::sendNotification);
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
    mCycleLenHintSlider.setValue(600);
    mCycleLenHintSlider.addListener(this);
    addAndMakeVisible(&mCycleLenHintSlider);

    mCycleLenHintSliderLabel.setText("Zoom", juce::dontSendNotification);
    mCycleLenHintSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(&mCycleLenHintSliderLabel);

    // Band Slider and Label
    mBandSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mBandSlider.setRange(0, 9, 1);
    mBandSlider.setValue(0);
    mBandSlider.addListener(this);
    addAndMakeVisible(&mBandSlider);

    mBandSliderLabel.setText("Band", juce::dontSendNotification);
    mBandSliderLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(&mBandSliderLabel);

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

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if(mPlaybackState == PlaybackStates::PlayingResampled)
        pPlayback->readSamplesFromVector(bufferToFill, mResampledCycles);
    else if (mPlaybackState == PlaybackStates::PlayingResynthesized)
        pPlayback->readSamplesFromVector(bufferToFill, mResynthesizedCycles);
    else
        bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    grabKeyboardFocus();
}

void MainComponent::resized()
{
    float aAreaY = 0;
    float bAreaY = 150;
    float cAreaY = 550;
    float aAreaHeight = 150;
    float bAreaHeight = 400;
    float cAreaHeight = 50;
    float width = getWidth();
    float gap = 5;
    float buttonsYPosition = 50;
    float bigButtonWidth = 86;
    float bigButtonHeight = 30;

    mWaveformDisplay.setBounds(0, bAreaY, width, bAreaHeight);
    mOriginalWaveform.setBounds(0, cAreaY, width, cAreaHeight);
    
    mModeOrigButton.setBounds(gap, 50, bigButtonWidth, bigButtonHeight);
    mModeResampledButton.setBounds(gap + bigButtonWidth + 2, 50, bigButtonWidth, bigButtonHeight);
    mModeResynthesizedButton.setBounds(gap + bigButtonWidth * 2 + 4, 50, bigButtonWidth, bigButtonHeight);
    
    mPrevCycleButton.setBounds(gap, 82, 20, 20);
    mPrevSampleButton.setBounds(gap + 22, 82, 20, 20);
    mNextSampleButton.setBounds(gap + 44, 82, 20, 20);
    mNextCycleButton.setBounds(gap + 66, 82, 20, 20);

    // SLIDERS
    mBandSliderLabel.setBounds(gap + 140, 105, 100, 10);
    mBandSlider.setBounds(gap, 110, 262, 20);
    mCycleLenHintSliderLabel.setBounds(gap + 140, 125, 100, 10);
    mCycleLenHintSlider.setBounds(gap, 130, 262, 20);

    // the mStartSampleIndexSlider should overlap mOriginalWaveform
    mStartSampleIndexSlider.setBounds(0, cAreaY, width, cAreaHeight);

    // Commit, Save, Clear, Delete
    mCommitButton.setBounds(553, 50, 58, 30);
    mSaveButton.setBounds(613, 50, 58, 30);
    mClearButton.setBounds(673, 50, 58, 30);
    mDeleteButton.setBounds(733, 50, 58, 30);
    
    mCycleLengthComboBox.setBounds(733, 82, 58, 20);
    
    mEventConfirmationLabel.setBounds(360, bAreaY, width - 300, 16);
    mResampledLengthLabel.setBounds(280, cAreaY - 10, width - 300, 10);
//    mInstructionsLabel.setBounds(10, 40, getWidth(), getHeight());
    
    mPlayButton.setBounds(360, 50, 80, 50);
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &mStartSampleIndexSlider){
        mStartSampleIndex = mStartSampleIndexSlider.getValue();
        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                    mClosestZeroCrossingStart,
                                                                    mClosestZeroCrossingEnd,
                                                                    mStartSampleIndex,
                                                                    mCycleLenHint);
        mEventConfirmationLabel.setVisible(false);
    } else if (slider == &mCycleLenHintSlider){
        // update the mStartSampleIndex so that we keep the waveform centered while zooming

        int newHint = mCycleLenHintSlider.getValue();
        int diff = newHint - mCycleLenHint;
        int halfDiff = diff / 2.0f;
        
        mStartSampleIndexSlider.setRange(0, mOrigAudioData.size() - newHint * 2);
        mStartSampleIndexSlider.setValue(mStartSampleIndexSlider.getValue() - halfDiff, juce::sendNotification);

        // update mCycleLenHint and find new zeroCrossings start and end points
        mCycleLenHint = mCycleLenHintSlider.getValue();
        pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                    mClosestZeroCrossingStart,
                                                                    mClosestZeroCrossingEnd,
                                                                    mStartSampleIndex,
                                                                    mCycleLenHint);

        mEventConfirmationLabel.setVisible(false);
    }
    repaint();
}

void MainComponent::comboBoxChanged(juce::ComboBox* box)
{
    if (box == &mCycleLengthComboBox)
    {
        int newValue = box->getText().getIntValue();
        std::cout << "New WTSIZE = " << newValue << std::endl;
        WTSIZE = newValue;
    }
}

void MainComponent::buttonClicked(juce::Button* button){
    if (button == &mCommitButton){
        handleCommitButton();
    } else if (button == &mSaveButton){
        pFileHandler->saveVectorAsAudioFileToDesktop(mResampledCycles, "resampled");
        pFileHandler->saveVectorAsAudioFileToDesktop(mPolarCycles, "polar");
        pFileHandler->saveVectorAsAudioFileToDesktop(mResynthesizedCycles, "resynthesized");

        mEventConfirmationLabel.setText("File Saved!", juce::dontSendNotification);
        mEventConfirmationLabel.setVisible(true);
    } else if (button == &mClearButton){
        mResampledCycles.clear();
        mPolarCycles.clear();
        mResynthesizedCycles.clear();
        updateLengthInfoLabel();
        mVectorThatShowsWhichSamplesAreCommitted.assign(mVectorThatShowsWhichSamplesAreCommitted.size(), false);
    } else if (button == &mDeleteButton){
    } else if (button == &mPlayButton){
        bool playButtonState = mPlayButton.getToggleState();
        
        if(playButtonState){
            mPlaybackState = PlaybackStates::PlayingResynthesized;
        } else {
            mPlaybackState = PlaybackStates::Stopped;
        }
    } else if(button == &mModeOrigButton){
        mModeOrigButton.setToggleState(true, juce::dontSendNotification);
        mModeResampledButton.setToggleState(false, juce::dontSendNotification);
        mModeResynthesizedButton.setToggleState(false, juce::dontSendNotification);
        mWaveformDisplay.setAudioVector(&mOrigAudioData, true);
        mOriginalWaveform.setAudioVector(&mOrigAudioData, false);
        repaint();
    } else if(button == &mModeResampledButton){
        mModeOrigButton.setToggleState(false, juce::dontSendNotification);
        mModeResampledButton.setToggleState(true, juce::dontSendNotification);
        mModeResynthesizedButton.setToggleState(false, juce::dontSendNotification);
    } else if(button == &mModeResynthesizedButton){
        mModeOrigButton.setToggleState(false, juce::dontSendNotification);
        mModeResampledButton.setToggleState(false, juce::dontSendNotification);
        mModeResynthesizedButton.setToggleState(true, juce::dontSendNotification);
        mWaveformDisplay.setAudioVector(&mResynthesizedCycles, false);
        mOriginalWaveform.setAudioVector(&mResynthesizedCycles, false);
        repaint();
    } else if(button == &mPrevCycleButton){
        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
        int newVal = mStartSampleIndexSlider.getValue() - cycleLen;
        if(newVal < 0)
            newVal = 0;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    } else if(button == &mPrevSampleButton){
        int newVal = mStartSampleIndexSlider.getValue() - 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    } else if(button == &mNextSampleButton){
        int newVal = mStartSampleIndexSlider.getValue() + 2;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    } else if(button == &mNextCycleButton){
        int cycleLen = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;
        int newVal = mStartSampleIndexSlider.getValue() + cycleLen;
        if(newVal > mOrigAudioData.size() - cycleLen)
            newVal = static_cast<int>(mOrigAudioData.size()) - cycleLen;
        mStartSampleIndexSlider.setValue(newVal, juce::sendNotificationSync);
    }
}

bool MainComponent::keyPressed(const juce::KeyPress& key, Component* originatingComponent){
    if (key.getTextCharacter() == 'C' || key.getTextCharacter() == 'c')
    {
        handleCommitButton();
        return true;
    } else if (key.getTextCharacter() == 'H' || key.getTextCharacter() == 'h'){
        mPrevCycleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'L' || key.getTextCharacter() == 'l'){
        mNextCycleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'J' || key.getTextCharacter() == 'j'){
        mPrevSampleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'K' || key.getTextCharacter() == 'k'){
        mNextSampleButton.triggerClick();
        return true;
    } else if (key.getTextCharacter() == 'I' || key.getTextCharacter() == 'i'){
        int newVal = mCycleLenHintSlider.getValue() - 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    } else if (key.getTextCharacter() == 'U' || key.getTextCharacter() == 'u'){
        int newVal = mCycleLenHintSlider.getValue() + 4;
        mCycleLenHintSlider.setValue(newVal, juce::sendNotificationSync);
        return true;
    }
    return false;
}

void MainComponent::handleCommitButton(){
    const float* mAudioBufferData = mOrigAudioData.data();

    int originalLengthOfCycle = mClosestZeroCrossingEnd - mClosestZeroCrossingStart;

    // set to true all indexed from the original file that are going to be used
    for(int i = mClosestZeroCrossingStart; i < mClosestZeroCrossingEnd; ++i){
        mVectorThatShowsWhichSamplesAreCommitted[i] = true;
    }

    // make a temp origCycle and copy the sampled from orig file
    std::vector<float> origCycle(originalLengthOfCycle, 0.0f);
    for(int i = 0; i < originalLengthOfCycle; ++i){
        origCycle[i] = mAudioBufferData[i + mClosestZeroCrossingStart];
    }
    
    /* WIP start ---------------

    std::vector<float> origCycle(1024, 0.0f);
    juce::File audioFile = juce::File("/Users/mihaitraista/4.Projects/Coding/JUCE/BLResampler/Resources/cello-res-dephased.wav");

    juce::AudioBuffer<float> tempBuff;
    pFileHandler->storeAudioFileInBuffer(audioFile, tempBuff);

    const float * tempBuffData = tempBuff.getReadPointer(0);
    std::cout << "duration = " << tempBuff.getNumSamples() << std::endl;

    for(int i = 0; i < 1024; ++i){
        origCycle[i] = tempBuffData[i];
    }

    mClosestZeroCrossingStart = 0;
    mClosestZeroCrossingEnd = 1023;

    WIP end --------------- */

    // create the three temp vectors
    std::vector<float> resampled = std::vector<float>(WTSIZE, 0.0f);
    std::vector<float> polarValues = std::vector<float>(WTSIZE * 2, 0.0f);
    std::vector<float> resynthesized = std::vector<float>(WTSIZE, 0.0f);

    pResampler->resizeCycle(origCycle, resampled);
    
    Fourier::fillDftPolar(resampled, polarValues);

    // harmonicsLimit = 512 because Niquist(22050) / freq(43) = 512 (1024 samples is 43 Hz)
    int harmonicsLimit = 512;

    Fourier::idft(polarValues, resynthesized, harmonicsLimit, 50.0f, 1);

//    pFileHandler->saveVectorAsAudioFileToDesktop(resynthesized, "resynthesized-single-cycle");
    
//    Fourier::rotateWavetableToNearestZero(resynthesized);

    // append the temp vectors to the member vectors
    mResampledCycles.insert(mResampledCycles.end(), resampled.begin(), resampled.end());
    mPolarCycles.insert(mPolarCycles.end(), polarValues.begin(), polarValues.end());
    mResynthesizedCycles.insert(mResynthesizedCycles.end(), resynthesized.begin(), resynthesized.end());

    updateLengthInfoLabel();

    mEventConfirmationLabel.setText("Cycle Committed!", juce::dontSendNotification);
    mEventConfirmationLabel.setVisible(true);

    repaint();
}

void MainComponent::updateLengthInfoLabel(){
    int len = static_cast<int>(mResampledCycles.size());
    juce::String labelText = "Resampled Buffer Length: " + juce::String(len) + " samples, " + juce::String(len / static_cast<float>(WTSIZE)) + " cycles";
    mResampledLengthLabel.setText(labelText, juce::dontSendNotification);
}

void MainComponent::filesDropped (const juce::StringArray& files, int x, int y)
{
    std::cout << "filesDropped" << std::endl;

    juce::File audioFile(files[0]);
    
    updateBufferAndRecalculateZeroCrossings(audioFile);
}

void MainComponent::updateBufferAndRecalculateZeroCrossings(juce::File& audioFile){
    pFileHandler->readAudioFileAndCopyToVector(audioFile, mOrigAudioData);
    
    mVectorThatShowsWhichSamplesAreCommitted.resize(mOrigAudioData.size(), false);
    
    pZeroCrossingFinder->calculateZeroCrossings(mOrigAudioData, mZeroCrossings);

    pZeroCrossingFinder->findClosestZeroCrossingsToCycleLenHint(mZeroCrossings,
                                                                mClosestZeroCrossingStart,
                                                                mClosestZeroCrossingEnd,
                                                                mStartSampleIndex,
                                                                mCycleLenHint);

    mStartSampleIndexSlider.setRange(0, mOrigAudioData.size() - mCycleLenHint * 2);
    mStartSampleIndexSlider.setValue(mOrigAudioData.size() / 2, juce::sendNotification);
}
